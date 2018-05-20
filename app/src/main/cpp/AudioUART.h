//
// Created by wehr_ on 5/6/2018.
//

#ifndef AUDIOSERIAL_AUDIOUART_H
#define AUDIOSERIAL_AUDIOUART_H

#include "common.h"

#include <oboe/Oboe.h>

#include <vector>
#include <array>
#include <deque>
#include <mutex>
#include <limits>
#include <type_traits>
#include <cstdint>
#include <cassert>
#include <cmath>

// Allows us to specify an integral type that has at least as many bits as N
// From https://stackoverflow.com/questions/38852678/c-integer-type-twice-the-width-of-a-given-type
template <size_t N> struct uint_by_size : uint_by_size<N+1> {};
template <size_t N> using uint_by_size_t = typename uint_by_size<N>::type;
template <class T> struct tag {using type = T; };
template <> struct uint_by_size<8>  : tag<uint8_t> {};
template <> struct uint_by_size<16>  : tag<uint16_t> {};

template <int DATA_BITS>
class AudioUART : public oboe::AudioStreamCallback{
public:
    AudioUART(const int baud_rate, const int stop_bits = 1);
    ~AudioUART();

    // uint8_t if DATA_BITS is 7 or 8, uint16_t if DATA_BITS is 9
    typedef uint_by_size_t<DATA_BITS> data_t;

    void sendByte(const data_t b);
    int start();
    int stop();
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audio_stream, void *audio_data, int32_t num_frames);
private:
    // constants for serial format
    const int BAUD_RATE;
    const int STOP_BITS;
    const int START_BITS = 1;

    oboe::AudioStream* stream;
    std::deque<data_t> data_q;
    std::array<std::vector<int16_t>, (1 << DATA_BITS)> bytes_wavetable;
    const int16_t MAXIMUM_AMPLITUDE_VALUE = std::numeric_limits<int16_t>::max();
    // Number of samples per serial frame
    int frame_width;

    // Builds the wave table for the specified serial protocol, and stream properties
    void createWaveTable();

    // protects the data queue from being added to and read at the same time
    std::mutex data_q_mut;
};


/*
 * -----------------------------------------------------------------
 * Implementation
 * -----------------------------------------------------------------
 */

template <int DATA_BITS>
AudioUART<DATA_BITS>::AudioUART(const int baud_rate, const int stop_bits)
        : BAUD_RATE(baud_rate)
        , STOP_BITS(stop_bits) {
    // build audio stream
    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Output);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setChannelCount(1);
    builder.setSampleRate(48000);
    builder.setFormat(oboe::AudioFormat::I16);

    builder.setCallback(this);

    auto result = builder.openStream(&stream);
    if (result != oboe::Result::OK) {
        LOGE("Failed to open Oboe stream. Error: %s", oboe::convertToText(result));
        return;
    }

    // build wave table now that stream is defined
    createWaveTable();
}

template <int DATA_BITS>
AudioUART<DATA_BITS>::~AudioUART() {
    stream->close();
}

template <int DATA_BITS>
int AudioUART<DATA_BITS>::start() {
    return stream->requestStart() == oboe::Result::OK;
}

template <int DATA_BITS>
int AudioUART<DATA_BITS>::stop() {
    return stream->requestStop() == oboe::Result::OK;
}

template <int DATA_BITS>
void AudioUART<DATA_BITS>::createWaveTable() {
    // Could be made to work with floats, just not implemented
    assert(stream->getFormat() == oboe::AudioFormat::I16);
    int sample_rate = stream->getSampleRate();

    // Number of bits in each byte frame (start bits, stop bits, data)
    int byte_frame_bits = DATA_BITS + START_BITS + STOP_BITS;
    // Number of samples per UART bit
    float bit_width = sample_rate / BAUD_RATE;
    // Number of samples per byte frame
    frame_width = std::ceil(byte_frame_bits * bit_width);

    // Fill out bytes wavetable
    for (int byte = 0; byte < bytes_wavetable.size(); byte++) {
        auto& wave = bytes_wavetable[byte];
        wave.resize(frame_width);

        for (int i = 0; i < frame_width; i++) {
            int bit_id = std::floor((float) i / bit_width - START_BITS);
            int16_t to_send;
            // start bits
            if (bit_id < 0) {
                to_send = -MAXIMUM_AMPLITUDE_VALUE;
            }
                // data bits
            else if (bit_id < DATA_BITS && bit_id >= 0) {
                // 0/1 bit to send
                uint8_t bit_send = (byte >> bit_id) & (0x01);
                // 0/1 expanded to minimum or maximum amplitude
                to_send = ((2 * bit_send) - 1) * MAXIMUM_AMPLITUDE_VALUE;
            }
                // stop bit and holding line high to indicate no data
            else {
                to_send = MAXIMUM_AMPLITUDE_VALUE;
            }
            wave[i] = to_send;
        }
    }
}

template <int DATA_BITS>
void AudioUART<DATA_BITS>::sendByte(const data_t b) {
    data_q.push_back(b);
}

template <int DATA_BITS>
oboe::DataCallbackResult AudioUART<DATA_BITS>::onAudioReady(oboe::AudioStream *audio_stream, void *audio_data, int32_t num_frames) {
    std::lock_guard<std::mutex> lock(data_q_mut);

    int16_t* out_buffer = static_cast<int16_t*>(audio_data);
    int left_to_fill = num_frames;
    while (left_to_fill) {
        if (data_q.size() && left_to_fill >= frame_width) {
            const data_t& b = data_q.front();
            memcpy(out_buffer, &bytes_wavetable[b][0], frame_width * sizeof(*out_buffer));
            out_buffer += frame_width;
            left_to_fill -= frame_width;
            data_q.pop_front();
        }
        else {
            // hold line high if no data
            out_buffer[0] = MAXIMUM_AMPLITUDE_VALUE;
            out_buffer++;
            left_to_fill--;
        }
    }
    return oboe::DataCallbackResult::Continue;
}
#endif //AUDIOSERIAL_AUDIOUART_H

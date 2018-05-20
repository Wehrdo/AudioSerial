#include "AudioUART.h"

#include <jni.h>

// 8 data bits, 9600 baud, default of 1 stop bit
static AudioUART<8> uart(9600);

static int startAudio() {
    return uart.start();
}

static int stopAudio() {
    return uart.stop();
}

static void sendByte(uint8_t byte) {
    uart.sendByte(byte);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_davidawehr_audioserial_AudioJNI_sendValue(JNIEnv *env, jclass type, jbyte val) {
    sendByte((uint8_t)val);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_davidawehr_audioserial_AudioJNI_startAudio(JNIEnv *env, jclass type) {
    return startAudio();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_davidawehr_audioserial_AudioJNI_stopAudio(JNIEnv *env, jclass type) {
    return uart.stop();
}

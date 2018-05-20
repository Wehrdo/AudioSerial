package com.davidawehr.audioserial;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import java.util.Locale;

public class MainActivity extends AppCompatActivity {
    SeekBar slider;
    Switch sw;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // stream switch toggled callback
        sw = findViewById(R.id.streamSwitch);
        sw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    AudioJNI.startAudio();
                }
                else {
                    AudioJNI.stopAudio();
                }
                slider.setEnabled(isChecked);
            }
        });

        final TextView valLabel = findViewById(R.id.valLabel);

        slider = findViewById(R.id.valBar);
        slider.setMax(255);
        slider.setProgress(255 / 2);
        slider.setEnabled(false);
        slider.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                byte shiftedProgress = (byte) (progress - 128);
                AudioJNI.sendValue(shiftedProgress);

                valLabel.setText(String.format(Locale.US, "Value: %d", (int)shiftedProgress));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
    }

    @Override
    protected void onStop() {
        super.onStop();
//        AudioJNI.stopAudio();
        sw.setChecked(false);
    }

}

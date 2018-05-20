package com.davidawehr.audioserial;

public class AudioJNI {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public static native int startAudio();
    public static native int stopAudio();

    public static native void sendValue(byte val);
}

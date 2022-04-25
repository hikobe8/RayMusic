package com.ray.musicplayer;

public class JniTest {

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
    }


    public native String stringFromJNI();

    public native void checkFFMPEG();

}

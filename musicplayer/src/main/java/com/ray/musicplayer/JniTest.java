package com.ray.musicplayer;

public class JniTest {

    static {
        System.loadLibrary("native-lib");
    }

    public  native String stringFromJNI();

}

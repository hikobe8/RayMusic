package com.ray.musicplayer;

import android.util.Log;

public class RayPlayer {

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
    }

    public native void native_prepare(String url);

    public void onPreparedFromJni() {
        Log.i("hikobe8", "player prepared!");
        native_start();
    }

    public native void native_start();
}

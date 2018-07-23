package com.ray.musicplayer;

/***
 *  Author : ryu18356@gmail.com
 *  Create at 2018-07-23 14:38
 *  description : 
 */
public class Demo {

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("postproc-54");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avdevice-57");
    }

    public native String stringFromJNI();

}

package com.ray.musicplayer;

public class RayPlayer {

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
    }

    private PlayerListener mPlayerListener;

    public void setPlayerListener(PlayerListener playerListener) {
        mPlayerListener = playerListener;
    }

    public void onPreparedFromJni() {
        if (null != mPlayerListener) {
            mPlayerListener.onPlayerPrepared();
        }
    }

    public void onLoadingFromJni(boolean loading) {
        if (null != mPlayerListener) {
            mPlayerListener.onPlayerLoading(loading);
        }
    }

    public void onPausedFromNative(){
        if (null != mPlayerListener) {
            mPlayerListener.onPlayerPause();
        }
    }

    public void onResumeFromNative(){
        if (null != mPlayerListener) {
            mPlayerListener.onPlayerResume();
        }
    }

    public native void native_start();
    public native void native_prepare(String url);
    public native void native_pause();
    public native void native_resume();
}

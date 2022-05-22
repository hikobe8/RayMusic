package com.ray.musicplayer;

import com.ray.bean.TimeInfo;

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

    private String url;

    public void setDataSource(String url) {
        this.url = url;
    }

    public void prepare() {
        native_prepare(url);
    }

    public void start() {
        native_start();
    }

    public void pause() {
        native_pause();
    }

    public void resume() {
        native_resume();
    }

    public void stop() {
        native_stop();
    }

    public void seek(int seconds) {
        native_seek(seconds);
    }

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

    public void onPausedFromNative() {
        if (null != mPlayerListener) {
            mPlayerListener.onPlayerPause();
        }
    }

    public void onResumeFromNative() {
        if (null != mPlayerListener) {
            mPlayerListener.onPlayerResume();
        }
    }

    public void onPlayerTimeChangeFromNative(int current, int total) {
        if (null != mPlayerListener) {
            TimeInfo timeInfo = new TimeInfo();
            timeInfo.current = current;
            timeInfo.total = total;
            mPlayerListener.onPlayerTimeChange(timeInfo);
        }
    }

    public void onErrorFromNative(int code, String msg) {
        if (null != mPlayerListener) {
            native_stop();
            mPlayerListener.onError(code, msg);
        }
    }

    private native void native_start();

    private native void native_prepare(String url);

    private native void native_pause();

    private native void native_resume();

    private native void native_stop();

    private native void native_seek(int seconds);

}

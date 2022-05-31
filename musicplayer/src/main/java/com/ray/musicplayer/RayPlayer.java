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

    private boolean playNext = false;

    private int mDuration;

    private int volumePercent = 100;

    private @ChannelMode
    int channelMode = ChannelMode.CHANNEL_LEFT;

    public void setDataSource(String url) {
        this.url = url;
    }

    public void prepare() {
        native_prepare(url);
    }

    public void start() {
        setVolumePercent(volumePercent);
        setChannelMode(channelMode);
        native_start();
    }

    public void pause() {
        native_pause();
    }

    public void resume() {
        native_resume();
    }

    public void stop() {
        mDuration = 0;
        native_stop();
    }

    public void seek(int seconds) {
        native_seek(seconds);
    }

    public void setPlayerListener(PlayerListener playerListener) {
        mPlayerListener = playerListener;
    }

    public void playNext(String url) {
        this.url = url;
        stop();
        playNext = true;
    }

    public int getDuration() {
        return mDuration;
    }

    public void setVolumePercent(int volumePercent) {
        if (volumePercent >= 0 && volumePercent <= 100) {
            this.volumePercent = volumePercent;
            native_setVolume(this.volumePercent);
        }
    }

    public void setChannelMode(@ChannelMode int channelMode) {
        this.channelMode = channelMode;
        native_setChannel(channelMode);
    }

    public void onPreparedFromJni() {
        if (null != mPlayerListener) {
            mPlayerListener.onPlayerPrepared();
        }
        mDuration = native_duration();
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
            stop();
            mPlayerListener.onError(code, msg);
        }
    }

    public void onCompleteFromNative() {
        if (null != mPlayerListener) {
            stop();
            mPlayerListener.onComplete();
        }
    }

    public void onPlayNextFromNative() {
        if (playNext) {
            playNext = false;
            prepare();
        }
    }

    private native void native_start();

    private native void native_prepare(String url);

    private native void native_pause();

    private native void native_resume();

    private native void native_stop();

    private native void native_seek(int seconds);

    private native int native_duration();

    private native void native_setVolume(int volume);

    private native void native_setChannel(int mode);

    public int getVolumePercent() {
        return volumePercent;
    }
}

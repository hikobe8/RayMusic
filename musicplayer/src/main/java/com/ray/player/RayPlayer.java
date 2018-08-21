package com.ray.player;

import android.text.TextUtils;

import com.ray.entity.TimeInfo;
import com.ray.listener.OnCompleteListener;
import com.ray.listener.OnErrorListener;
import com.ray.listener.OnLoadListener;
import com.ray.listener.OnPauseResumeListener;
import com.ray.listener.PlayTimeListener;
import com.ray.listener.PlayerPrepareListener;
import com.ray.log.MyLog;
import com.ray.type.ChannelType;

/***
 *  Author : ryu18356@gmail.com
 *  Create at 2018-08-07 18:44
 *  description : 
 */
public class RayPlayer {

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

    private String mSource;
    private PlayerPrepareListener mPlayerPrepareListener;
    private OnLoadListener mOnLoadListener;
    private OnPauseResumeListener mOnPauseResumeListener;
    private PlayTimeListener mPlayTimeListener;
    private OnErrorListener mOnErrorListener;
    private OnCompleteListener mOnCompleteListener;

    private static TimeInfo sTimeInfo;
    private static boolean sPlayNext;
    private static int sDuration = -1;
    private static int sVolumePercent = 50;
    private static float sPitch = 1.0f;
    private static float sSpeed = 1.0f;

    public void setPlayerPrepareListener(PlayerPrepareListener playerPrepareListener) {
        mPlayerPrepareListener = playerPrepareListener;
    }

    public void setOnLoadListener(OnLoadListener onLoadListener) {
        mOnLoadListener = onLoadListener;
    }

    public void setOnPauseResumeListener(OnPauseResumeListener onPauseResumeListener) {
        mOnPauseResumeListener = onPauseResumeListener;
    }

    public void setPlayTimeListener(PlayTimeListener playTimeListener) {
        mPlayTimeListener = playTimeListener;
    }

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        mOnErrorListener = onErrorListener;
    }

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        mOnCompleteListener = onCompleteListener;
    }

    public void setSource(String source) {
        mSource = source;
    }

    public void prepare() {
        if (TextUtils.isEmpty(mSource)) {
            MyLog.e("play source can't be empty!");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                native_prepare(mSource);
            }
        }).start();
    }

    public void start() {
        if (TextUtils.isEmpty(mSource)) {
            MyLog.e("play source can't be empty!");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                native_start();
            }
        }).start();
    }

    public void pause() {
        if (mOnPauseResumeListener != null) {
            mOnPauseResumeListener.onPause();
        }
        native_pause();
    }

    public void resume() {
        if (mOnPauseResumeListener != null) {
            mOnPauseResumeListener.onResume();
        }
        native_resume();
    }

    public void stop() {
        sTimeInfo = null;
        new Thread(new Runnable() {
            @Override
            public void run() {
                native_stop();
            }
        }).start();
    }

    public void seek(int seconds) {
        native_seek(seconds);
    }

    public void playNext(String url) {
        sPlayNext = true;
        setSource(url);
        stop();
    }

    public int getDuration() {
        if (sDuration < 0) {
            sDuration = native_getDuration();
        }
        return sDuration;
    }

    public int getVolume() {
        return sVolumePercent;
    }

    public void setVolume(int volumePercent) {
        if (volumePercent >= 0 && volumePercent <= 100) {
            sVolumePercent = volumePercent;
            native_setVolume(volumePercent);
        }
    }

    public void setChannelType(@ChannelType int type){
        native_setChannelType(type);
    }

    public void setPitch(float pitch){
        sPitch = pitch;
        native_setPitch(pitch);
    }

    public void setSpeed(float speed){
        sSpeed = speed;
        native_setSpeed(speed);
    }

    public void onCallPrepared() {
        if (mPlayerPrepareListener != null)
            mPlayerPrepareListener.onPrepared();
    }

    public void onResourceLoaded(boolean isLoading) {
        if (mOnLoadListener != null) {
            mOnLoadListener.onLoad(isLoading);
        }
    }

    public void onPlayTimeChanged(int nowTime, int duration) {
        if (mPlayTimeListener != null) {
            if (sTimeInfo == null) {
                sTimeInfo = new TimeInfo(nowTime, duration);
            }
            sTimeInfo.nowTime = nowTime;
            sTimeInfo.duration = duration;
            mPlayTimeListener.onPlayTimeChanged(sTimeInfo);
        }
    }

    public void onErrorCall(int code, String msg) {
        stop();
        if (mOnErrorListener != null) {
            mOnErrorListener.onError(code, msg);
        }
    }

    public void onCallComplete() {
        stop();
        if (mOnCompleteListener != null) {
            mOnCompleteListener.onComplete();
        }
    }

    public void onCallNext() {
        if (sPlayNext) {
            sPlayNext = false;
            prepare();
        }
    }

    private native void native_prepare(String source);

    private native void native_start();

    private native void native_pause();

    private native void native_resume();

    private native void native_stop();

    private native void native_seek(int seconds);

    private native int native_getDuration();

    private native void native_setVolume(int volumePercent);

    private native void native_setChannelType(int type);

    private native void native_setPitch(float pitch);

    private native void native_setSpeed(float speed);
}

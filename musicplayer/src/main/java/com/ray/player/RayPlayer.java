package com.ray.player;

import android.text.TextUtils;

import com.ray.listener.PlayerPrepareListener;
import com.ray.log.MyLog;

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

    public void setPlayerPrepareListener(PlayerPrepareListener playerPrepareListener) {
        mPlayerPrepareListener = playerPrepareListener;
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

    public void onCallPrepared() {
        if (mPlayerPrepareListener != null)
            mPlayerPrepareListener.onPrepared();
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

    private native void native_prepare(String source);

    private native void native_start();

}

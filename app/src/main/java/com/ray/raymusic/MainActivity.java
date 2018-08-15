package com.ray.raymusic;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.ray.TimeUtil;
import com.ray.entity.TimeInfo;
import com.ray.listener.OnLoadListener;
import com.ray.listener.OnPauseResumeListener;
import com.ray.listener.PlayTimeListener;
import com.ray.listener.PlayerPrepareListener;
import com.ray.log.MyLog;
import com.ray.player.RayPlayer;

import java.io.File;
import java.lang.ref.WeakReference;

public class MainActivity extends AppCompatActivity {


    private static final boolean TEST_NET_SWITCH = true;
    private static final int KEY_UPDATE_PLAY_TIME = 1;

    private static final String TEST_FILE = "output.mp3";
    private RayPlayer mPlayer;
    private TextView mTvTime;
    private MHandler mMHandler;

    static class MHandler extends Handler {
        WeakReference<MainActivity> mContextWeakReference;

        MHandler(MainActivity context) {
            mContextWeakReference = new WeakReference<>(context);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            MainActivity mainActivity = mContextWeakReference.get();
            if (mainActivity != null && msg.what == KEY_UPDATE_PLAY_TIME) {
                TimeInfo timeInfo= (TimeInfo) msg.obj;
                String nowTime = TimeUtil.getMMssTime(timeInfo.nowTime);
                String duration = TimeUtil.getMMssTime(timeInfo.duration);
                mainActivity.mTvTime.setText(mainActivity.getString(R.string.play_time, nowTime, duration));
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mTvTime = findViewById(R.id.tv_time);
        mMHandler = new MHandler(this);
    }

    public void start(View view) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            int permission = checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
            if (permission == PackageManager.PERMISSION_GRANTED) {
                prepare();
            } else {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
            }
        } else {
            prepare();
        }

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        prepare();
    }

    private void prepare() {
        mPlayer = new RayPlayer();
        mPlayer.setPlayerPrepareListener(new PlayerPrepareListener() {
            @Override
            public void onPrepared() {
                MyLog.d("准备完成 on Thread " + (Looper.myLooper() == Looper.getMainLooper() ? "main" : "child"));
                mPlayer.start();
            }
        });
        mPlayer.setOnLoadListener(new OnLoadListener() {
            @Override
            public void onLoad(boolean isLoading) {
                MyLog.d(isLoading ? " 加载中 " : " 播放中 ");
            }
        });
        mPlayer.setOnPauseResumeListener(new OnPauseResumeListener() {
            @Override
            public void onPause() {
                MyLog.d("暂停中...");
            }

            @Override
            public void onResume() {
                MyLog.d("播放中...");
            }

            @Override
            public void onStoped() {
                MyLog.d("停止播放");
            }
        });
        mPlayer.setPlayTimeListener(new PlayTimeListener() {
            @Override
            public void onPlayTimeChanged(TimeInfo timeInfo) {
                mMHandler.obtainMessage(KEY_UPDATE_PLAY_TIME, timeInfo).sendToTarget();
            }
        });
        if (TEST_NET_SWITCH) {
//            mPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
            mPlayer.setSource("http://live.hkstv.hk.lxdns.com/live/hks/playlist.m3u8");
        } else {
            mPlayer.setSource(Environment.getExternalStorageDirectory() + File.separator + TEST_FILE);
        }
        mPlayer.prepare();
    }

    public void pause(View view) {
        mPlayer.pause();
    }

    public void resume(View view) {
        mPlayer.resume();
    }

    public void stop(View view) {
        mPlayer.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMHandler.removeMessages(KEY_UPDATE_PLAY_TIME);
    }
}

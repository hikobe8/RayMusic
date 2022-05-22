package com.ray.app;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.View;
import android.widget.TextView;

import com.ray.bean.TimeInfo;
import com.ray.musicplayer.PlayerListener;
import com.ray.musicplayer.RayLog;
import com.ray.musicplayer.RayPlayer;
import com.ray.util.TimeUtil;

public class MainActivity extends AppCompatActivity implements PlayerListener {

    RayPlayer rayPlayer;
    private TextView tvProgress;
    private TextView tvTotal;

    Handler progressHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (1 == msg.what) {
                TimeInfo timeInfo = (TimeInfo) msg.obj;
                tvProgress.setText(TimeUtil.secondsToDateFormat(timeInfo.current, timeInfo.total));
                tvTotal.setText(TimeUtil.secondsToDateFormat(timeInfo.total, timeInfo.total));
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tvProgress = findViewById(R.id.tv_progress);
        tvTotal = findViewById(R.id.tv_total);
        rayPlayer = new RayPlayer();
        rayPlayer.setPlayerListener(this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
            }
        }
    }

    public void start(View view) {
//        rayPlayer.native_prepare("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        rayPlayer.setDataSource("http://ngcdn004.cnr.cn/live/dszs/index.m3u8");
        rayPlayer.prepare();
    }

    @Override
    public void onPlayerPrepared() {
        rayPlayer.start();
    }

    @Override
    public void onPlayerPause() {
        RayLog.d("暂停播放");
    }

    @Override
    public void onPlayerResume() {
        RayLog.d("继续播放");
    }

    @Override
    public void onPlayerLoading(boolean loading) {
        if (loading) {
            RayLog.d("加载中...");
        } else {
            RayLog.d("播放中...");
        }
    }

    @Override
    public void onPlayerTimeChange(TimeInfo timeInfo) {
        Message.obtain(progressHandler, 1, timeInfo).sendToTarget();
    }

    public void pause(View view) {
        rayPlayer.pause();
    }

    public void resume(View view) {
        rayPlayer.resume();
    }

    public void stop(View view) {
        rayPlayer.stop();
    }
}
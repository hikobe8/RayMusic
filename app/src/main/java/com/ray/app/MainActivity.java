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
import android.widget.SeekBar;
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
    private SeekBar progressBar;
    private TextView tvVolume;
    private SeekBar volumeBar;

    Handler progressHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (1 == msg.what) {
                TimeInfo timeInfo = (TimeInfo) msg.obj;
                tvProgress.setText(TimeUtil.secondsToDateFormat(timeInfo.current, timeInfo.total));
                tvTotal.setText(TimeUtil.secondsToDateFormat(timeInfo.total, timeInfo.total));
                if (timeInfo.total > 0) {
                    progressBar.setProgress((int) (timeInfo.current * 100f / timeInfo.total));
                }
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
        progressBar = findViewById(R.id.progressBar);
        progressBar.setMax(100);
        progressBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (fromUser) {
                    int seconds = (int) (progress * 1f / seekBar.getMax() * rayPlayer.getDuration());
                    rayPlayer.seek(seconds);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                rayPlayer.pause();
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                rayPlayer.resume();
            }
        });
        tvVolume = findViewById(R.id.tv_volume);
        volumeBar = findViewById(R.id.volumeBar);
        rayPlayer.setVolumePercent(50);
        tvVolume.setText("当前音量：" + rayPlayer.getVolumePercent());
        volumeBar.setProgress(rayPlayer.getVolumePercent());
        volumeBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                rayPlayer.setVolumePercent(progress);
                tvVolume.setText("当前音量：" + rayPlayer.getVolumePercent());
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public void start(View view) {
        rayPlayer.setDataSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
//        rayPlayer.setDataSource("http://ngcdn004.cnr.cn/live/dszs/index.m3u8");
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

    @Override
    public void onError(int code, String msg) {
        RayLog.e("error code = " + code + " , msg = " + msg);
    }

    @Override
    public void onComplete() {
        Message.obtain(progressHandler, 1, new TimeInfo()).sendToTarget();
        RayLog.i("播放完成");
    }

    public void pause(View view) {
        rayPlayer.pause();
    }

    public void resume(View view) {
        rayPlayer.resume();
    }

    public void stop(View view) {
        Message.obtain(progressHandler, 1, new TimeInfo()).sendToTarget();
        rayPlayer.stop();
        progressBar.setProgress(0);
    }

    public void next(View view) {
        rayPlayer.playNext("http://ngcdn004.cnr.cn/live/dszs/index.m3u8");
    }
}
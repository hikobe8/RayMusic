package com.ray.app;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;

import com.ray.musicplayer.PlayerListener;
import com.ray.musicplayer.RayLog;
import com.ray.musicplayer.RayPlayer;

public class MainActivity extends AppCompatActivity implements PlayerListener {

    RayPlayer rayPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        rayPlayer = new RayPlayer();
        rayPlayer.setPlayerListener(this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
            }
        }
    }

    public void start(View view) {
        rayPlayer.native_prepare("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
    }

    @Override
    public void onPlayerPrepared() {
        rayPlayer.native_start();
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

    public void pause(View view) {
        rayPlayer.native_pause();
    }

    public void resume(View view) {
        rayPlayer.native_resume();
    }
}
package com.ray.raymusic;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.ray.listener.OnLoadListener;
import com.ray.listener.OnPauseResumeListener;
import com.ray.listener.PlayerPrepareListener;
import com.ray.log.MyLog;
import com.ray.player.RayPlayer;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    private static final boolean TEST_NET_SWITCH = true;

    private static final String TEST_FILE = "output.mp3";
    private RayPlayer player;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
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
        player = new RayPlayer();
        player.setPlayerPrepareListener(new PlayerPrepareListener() {
            @Override
            public void onPrepared() {
                MyLog.d("准备完成 on Thread " + (Looper.myLooper() == Looper.getMainLooper() ? "main" : "child"));
                player.start();
            }
        });
        player.setOnLoadListener(new OnLoadListener() {
            @Override
            public void onLoad(boolean isLoading) {
                MyLog.d(isLoading ? " 加载中 " : " 播放中 ");
            }
        });
        player.setOnPauseResumeListener(new OnPauseResumeListener() {
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
        if (TEST_NET_SWITCH) {
            player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        } else {
            player.setSource(Environment.getExternalStorageDirectory() + File.separator + TEST_FILE);
        }
        player.prepare();
    }

    public void pause(View view) {
        player.pause();
    }

    public void resume(View view) {
        player.resume();
    }

    public void stop(View view) {
        player.stop();
    }
}

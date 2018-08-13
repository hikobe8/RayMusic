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

import com.ray.listener.PlayerPrepareListener;
import com.ray.log.MyLog;
import com.ray.player.RayPlayer;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    private static final boolean TEST_NET_SWITCH = false;

    private static final String TEST_FILE = "output.mp3";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void start(View view) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            int permission = checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE);
            if (permission == PackageManager.PERMISSION_GRANTED) {
                final RayPlayer player = new RayPlayer();
                player.setPlayerPrepareListener(new PlayerPrepareListener() {
                    @Override
                    public void onPrepared() {
                        MyLog.d("onPrepared on Thread " + (Looper.myLooper() == Looper.getMainLooper() ? "main" : "child"));
                        player.start();
                    }
                });
                if (TEST_NET_SWITCH) {
                    player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
                } else {
                    player.setSource(Environment.getExternalStorageDirectory() + File.separator + TEST_FILE);
                }
                player.prepare();
            } else {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 1);
            }
        } else {
            final RayPlayer player = new RayPlayer();
            player.setPlayerPrepareListener(new PlayerPrepareListener() {
                @Override
                public void onPrepared() {
                    MyLog.d("onPrepared on Thread " + (Looper.myLooper() == Looper.getMainLooper() ? "main" : "child"));
                    player.start();
                }
            });
            if (TEST_NET_SWITCH) {
                player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
            } else {
                player.setSource(Environment.getExternalStorageDirectory() + File.separator + TEST_FILE);
            }
            player.prepare();
        }

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        final RayPlayer player = new RayPlayer();
        player.setPlayerPrepareListener(new PlayerPrepareListener() {
            @Override
            public void onPrepared() {
                MyLog.d("onPrepared on Thread " + (Looper.myLooper() == Looper.getMainLooper() ? "main" : "child"));
                player.start();
            }
        });
        if (TEST_NET_SWITCH) {
            player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        } else {
            player.setSource(Environment.getExternalStorageDirectory() + File.separator + TEST_FILE);
        }
        player.prepare();
    }
}

package com.ray.app;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;

import com.ray.musicplayer.RayPlayer;

public class MainActivity extends AppCompatActivity {

    RayPlayer rayPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        rayPlayer = new RayPlayer();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
            }
        }
    }

    public void start(View view) {
        rayPlayer.native_prepare("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
    }
}
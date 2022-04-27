package com.ray.app;

import androidx.appcompat.app.AppCompatActivity;

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
    }

    public void prepare(View view) {
        rayPlayer.prepare("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
    }
}
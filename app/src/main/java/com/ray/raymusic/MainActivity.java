package com.ray.raymusic;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.ray.player.RayPlayer;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        RayPlayer player = new RayPlayer();
        player.setSource("");
        player.prepare();
    }

}

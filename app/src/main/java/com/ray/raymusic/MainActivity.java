package com.ray.raymusic;

import android.os.Bundle;
import android.os.Looper;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.ray.listener.PlayerPrepareListener;
import com.ray.log.MyLog;
import com.ray.player.RayPlayer;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void start(View view) {
        final RayPlayer player = new RayPlayer();
        player.setPlayerPrepareListener(new PlayerPrepareListener() {
            @Override
            public void onPrepared() {
                MyLog.d("onPrepared on Thread " + (Looper.myLooper() == Looper.getMainLooper() ? "main" : "child"));
                player.start();
            }
        });
        player.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        player.prepare();
    }
}

package com.ray.raymusic;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import com.ray.listener.PlayerPrepareListener;
import com.ray.player.RayPlayer;

public class CutAudioActivity extends AppCompatActivity {

    private RayPlayer mRayPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cut_audio);
        mRayPlayer = new RayPlayer();
        mRayPlayer.setPlayerPrepareListener(new PlayerPrepareListener() {
            @Override
            public void onPrepared() {
                mRayPlayer.cutAudioPlay(10, 25, true);
            }
        });
    }

    public void cutAudioPlay(View view) {
        mRayPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        mRayPlayer.prepare();
    }
}

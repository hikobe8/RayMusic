package com.ray.raymusic;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import com.ray.musicplayer.Demo;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Demo demo = new Demo();
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(demo.stringFromJNI());
    }

}

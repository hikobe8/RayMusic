package com.ray.cppthread;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

public class MainActivity extends AppCompatActivity {

    PThreadDemo pThreadDemo = new PThreadDemo();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void createNormalPThread(View view) {
        pThreadDemo.createNormalThread();
    }

    public void producerConsumer(View view) {
        pThreadDemo.producerConsumerModel();
    }
}
package com.ray.cppthread;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    PThreadDemo pThreadDemo = new PThreadDemo();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        pThreadDemo.setJavaCallback((code, msg) ->
                Toast.makeText(this, "code = " + code + " msg = " + msg, Toast.LENGTH_SHORT).show());
    }

    public void createNormalPThread(View view) {
        pThreadDemo.createNormalThread();
    }

    public void producerConsumer(View view) {
        pThreadDemo.producerConsumerModel();
    }


    public void cppCallJavaMainThread(View view) {
        pThreadDemo.cppCallJavaMainThread();
    }
}
package com.ray.cppthread;

public class PThreadDemo {

    static {
        System.loadLibrary("native-lib");
    }

    public native void createNormalThread();

}

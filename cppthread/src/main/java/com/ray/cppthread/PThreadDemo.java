package com.ray.cppthread;

public class PThreadDemo {

    static {
        System.loadLibrary("native-lib");
    }

    public native void createNormalThread();

    /**
     * C++ 实现生产者消费者模型
     */
    public native void producerConsumerModel();

}

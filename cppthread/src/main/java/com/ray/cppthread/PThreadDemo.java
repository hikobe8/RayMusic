package com.ray.cppthread;

import androidx.annotation.Keep;

public class PThreadDemo {

    static {
        System.loadLibrary("native-lib");
    }

    IJavaCallback iJavaCallback;

    public void setJavaCallback(IJavaCallback iJavaCallback) {
        this.iJavaCallback = iJavaCallback;
    }

    public interface IJavaCallback {
        void onNext(int code, String msg);
    }

    public native void createNormalThread();

    /**
     * C++ 实现生产者消费者模型
     */
    public native void producerConsumerModel();

    /**
     * C++回调Java方法(主线程)
     */
    public native void cppCallJavaMainThread();

    public void onNext(int code, String msg) {
        if (null != iJavaCallback)
            iJavaCallback.onNext(code, msg);
    }

}

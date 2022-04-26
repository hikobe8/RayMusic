package com.ray.cppthread;

import android.os.Handler;
import android.os.Looper;


public class PThreadDemo {

    static {
        System.loadLibrary("native-lib");
    }

    IJavaCallback iJavaCallback;

    private Handler mH = new Handler(Looper.getMainLooper());

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

    /**
     * C++回调Java方法(子线程)
     */
    public native void cppCallJavaChildThread();

    public void onNext(int code, String msg) {
        if (null != iJavaCallback) {
            if (Looper.myLooper() == Looper.getMainLooper()) {
                iJavaCallback.onNext(code, msg);
            } else {
                mH.post(() -> iJavaCallback.onNext(code, msg));
            }
        }
    }

}

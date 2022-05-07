package com.ray.musicplayer;

import android.util.Log;

/**
 * Author : Ray
 * Time : 2022/5/7 15:55
 * Description :
 */
public class RayLog {
    private static boolean ENABLED = true;
    private static String TAG = "hikobe8";

    public static void i(String msg) {
        Log.i(TAG, msg);
    }

    public static void w(String msg) {
        Log.w(TAG, msg);
    }

    public static void d(String msg) {
        Log.d(TAG, msg);
    }

    public static void e(String msg) {
        Log.e(TAG, msg);
    }

}

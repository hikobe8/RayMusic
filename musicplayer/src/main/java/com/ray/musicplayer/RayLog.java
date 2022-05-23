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
        if (!ENABLED)
            return;
        Log.i(TAG, msg);
    }

    public static void w(String msg) {
        if (!ENABLED)
            return;
        Log.w(TAG, msg);
    }

    public static void d(String msg) {
        if (!ENABLED)
            return;
        Log.d(TAG, msg);
    }

    public static void e(String msg) {
        if (!ENABLED)
            return;
        Log.e(TAG, msg);
    }

}

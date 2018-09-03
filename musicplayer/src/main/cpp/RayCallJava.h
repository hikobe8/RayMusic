//
// Created by Administrator on 2018/8/7.
//

#ifndef RAYMUSIC_RAYCALLJAVA_H
#define RAYMUSIC_RAYCALLJAVA_H

#define MAIN_THREAD 0
#define CHILD_THREAD 1

#include "jni.h"
#include <linux/stddef.h>
#include "AndroidLog.h"

class RayCallJava {

public:
    JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;
    jmethodID jMIDPrepare;
    jmethodID jMIDLoad;
    jmethodID jMIDTime;
    jmethodID jMIDCallError;
    jmethodID jMIDCallComplete;
    jmethodID jMIDCallDbValueChanged;
    jmethodID jMIDEncodePcm2Aac;
    jmethodID jMIDGetPcmCutInfo;
    jmethodID jMIDGetPcmCutInfoSampleRate;

public:
    RayCallJava(JavaVM *javaVM, JNIEnv *env, jobject obj);

    ~RayCallJava();

    void onCallPrepared(int type);

    void onLoad(int type, bool isLoading);

    void onTimeChanged(int type, int now_time, int duration);

    void onCallError(int type, int code, const char *msg);

    void onCallComplete(int type);

    void onDbValueChanged(int type, int db);

    void onCallRecord(int type, int size, void* buffer);

    void onGetPcmCutInfo(void* buffer, int size);

    void onGetPcmCutInfoSampleRate(int sampleRate);
};

#endif //RAYMUSIC_RAYCALLJAVA_H

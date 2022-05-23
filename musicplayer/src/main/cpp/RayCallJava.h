//
// Created by EDZ on 2022/4/27.
//

#ifndef RAYMUSIC_RAYCALLJAVA_H
#define RAYMUSIC_RAYCALLJAVA_H

#include <cwchar>
#include "jni.h"

#define CHILD_THREAD 0
#define MAIN_THREAD 1

class RayCallJava {

    JavaVM *javaVm = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;
    jmethodID jmidOnPrepared;
    jmethodID jmidOnLoading;
    jmethodID jmidOnPause;
    jmethodID jmidOnResume;
    jmethodID jmidOnProgressChange;
    jmethodID jmidOnError;
    jmethodID jmidOnComplete;
public:
    RayCallJava(JavaVM *vm, JNIEnv *env, jobject *obj);

    ~RayCallJava();

    void onCallPrepare(int type);

    void onCallLoading(int type, bool loading);

    void onCallPause(int type);

    void onCallResume(int type);

    void onCallProgressChange(int type, int progress, int total);

    void onCallError(int type, int code, char *msg);

    void onCallComplete(int type);

};


#endif //RAYMUSIC_RAYCALLJAVA_H

//
// Created by EDZ on 2022/4/27.
//

#ifndef RAYMUSIC_RAYCALLJAVA_H
#define RAYMUSIC_RAYCALLJAVA_H

#include "jni.h"

#define CHILD_THREAD 0
#define MAIN_THREAD 1

class RayCallJava {

    JavaVM *javaVm;
    JNIEnv *jniEnv;
    jobject jobj;
    jmethodID jmid;
public:
    RayCallJava(JavaVM *vm, JNIEnv *env, jobject obj);
    ~RayCallJava();
    void onCallPrepare(int type);
};


#endif //RAYMUSIC_RAYCALLJAVA_H

//
// Created by Administrator on 2018/8/7.
//

#ifndef RAYMUSIC_RAYCALLJAVA_H
#define RAYMUSIC_RAYCALLJAVA_H

#define MAIN_THEAD 0
#define CHILD_THEAD 1

#include "jni.h"
#include <linux/stddef.h>

class RayCallJava {

public:
    JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;
    jmethodID jMIDPrepare;

public:
    RayCallJava(JavaVM *javaVM, JNIEnv *env, jobject obj);

    ~RayCallJava();

    void onCallPrepared(int type);
};

#endif //RAYMUSIC_RAYCALLJAVA_H

//
// Created by Administrator on 2018/8/7.
//

#include "RayCallJava.h"

RayCallJava::RayCallJava(JavaVM *javaVM, JNIEnv *env, jobject obj) {
    this->javaVM = javaVM;
    this->jniEnv = env;
    this->jobj = obj;
    this->jobj = env->NewGlobalRef(jobj);

    jclass jclz = jniEnv->GetObjectClass(jobj);
    if (!jclz) {
        if (LOG_DEBUG) {
            LOGE("get jClass error!");
        }
        return;
    }
    jMIDPrepare = jniEnv->GetMethodID(jclz, "onCallPrepared", "()V");
    jMIDLoad = jniEnv->GetMethodID(jclz, "onResourceLoaded", "(Z)V");
    jMIDTime = jniEnv->GetMethodID(jclz, "onPlayTimeChanged", "(II)V");
}

RayCallJava::~RayCallJava() {
}

void RayCallJava::onCallPrepared(int type) {
    if (type == MAIN_THEAD) {
        jniEnv->CallVoidMethod(jobj, jMIDPrepare);
    } else if (type == CHILD_THEAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jniEnv error!");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jMIDPrepare);
        javaVM->DetachCurrentThread();
    }
}

void RayCallJava::onLoad(int type, bool isLoading) {
    if (type == MAIN_THEAD) {
        jniEnv->CallVoidMethod(jobj, jMIDLoad, isLoading);
    } else if (type == CHILD_THEAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jniEnv error!");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jMIDLoad, isLoading);
        javaVM->DetachCurrentThread();
    }
}

void RayCallJava::onTimeChanged(int type, int now_time, int duration) {
    if (type == MAIN_THEAD) {
        jniEnv->CallVoidMethod(jobj, jMIDTime, now_time, duration);
    } else if (type == CHILD_THEAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jniEnv error!");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jMIDTime, now_time, duration);
        javaVM->DetachCurrentThread();
    }
}

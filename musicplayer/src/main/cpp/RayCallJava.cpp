//
// Created by EDZ on 2022/4/27.
//
#include "RayCallJava.h"
#include "androidlog.h"

RayCallJava::RayCallJava(JavaVM *vm, JNIEnv *env, jobject obj) {
    javaVm = vm;
    jniEnv = env;
    jobj = env->NewGlobalRef(obj);
    jclass clz = env->GetObjectClass(jobj);
    //校验Java Class是否获取成功
    if (!clz) {
        LOGE("env->GetObjectClass error ");
        return;
    }
    jmidOnPrepared = env->GetMethodID(clz, "onPreparedFromJni", "()V");
    jmidOnLoading = env->GetMethodID(clz, "onLoadingFromJni", "(Z)V");
    jmidOnPause = env->GetMethodID(clz, "onPausedFromNative", "()V");
    jmidOnResume = env->GetMethodID(clz, "onResumeFromNative", "()V");
    jmidOnProgressChange = env->GetMethodID(clz, "onPlayerTimeChangeFromNative", "(II)V");
}

RayCallJava::~RayCallJava() {

}

void RayCallJava::onCallPrepare(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmidOnPrepared);
    } else {
        JNIEnv *jenv;
        if (javaVm->AttachCurrentThread(&jenv, 0) != JNI_OK) {
            LOGE("javaVm->AttachCurrentThread failed!");
            return;
        }
        jenv->CallVoidMethod(jobj, jmidOnPrepared);
        javaVm->DetachCurrentThread();
    }
}

void RayCallJava::onCallLoading(int type, bool loading) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmidOnLoading, loading);
    } else {
        JNIEnv *jenv;
        if (javaVm->AttachCurrentThread(&jenv, 0) != JNI_OK) {
            LOGE("javaVm->AttachCurrentThread failed!");
            return;
        }
        jenv->CallVoidMethod(jobj, jmidOnLoading, loading);
        javaVm->DetachCurrentThread();
    }
}

void RayCallJava::onCallPause(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmidOnPause);
    } else {
        JNIEnv *jenv;
        if (javaVm->AttachCurrentThread(&jenv, 0) != JNI_OK) {
            LOGE("javaVm->AttachCurrentThread failed!");
            return;
        }
        jenv->CallVoidMethod(jobj, jmidOnPause);
        javaVm->DetachCurrentThread();
    }
}

void RayCallJava::onCallResume(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmidOnResume);
    } else {
        JNIEnv *jenv;
        if (javaVm->AttachCurrentThread(&jenv, 0) != JNI_OK) {
            LOGE("javaVm->AttachCurrentThread failed!");
            return;
        }
        jenv->CallVoidMethod(jobj, jmidOnResume);
        javaVm->DetachCurrentThread();
    }
}

void RayCallJava::onCallProgressChange(int type, int progress, int total) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmidOnProgressChange, progress, total);
    } else {
        JNIEnv *jenv;
        if (javaVm->AttachCurrentThread(&jenv, 0) != JNI_OK) {
            LOGE("javaVm->AttachCurrentThread failed!");
            return;
        }
        jenv->CallVoidMethod(jobj, jmidOnProgressChange, progress, total);
        javaVm->DetachCurrentThread();
    }
}

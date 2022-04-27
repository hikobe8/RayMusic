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
    jmid = env->GetMethodID(clz, "onPreparedFromJni", "()V");
}

RayCallJava::~RayCallJava() {

}

void RayCallJava::onCallPrepare(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid);
    } else {
        JNIEnv *jenv;
        if (javaVm->AttachCurrentThread(&jenv, 0) != JNI_OK) {
            LOGE("javaVm->AttachCurrentThread failed!");
            return;
        }
        jenv->CallVoidMethod(jobj, jmid);
        javaVm->DetachCurrentThread();
    }
}

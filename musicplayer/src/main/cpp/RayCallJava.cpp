//
// Created by Administrator on 2018/8/7.
//

#include "RayCallJava.h"
#include "AndroidLog.h"

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

//
// Created by EDZ on 2022/4/26.
//

#include "javalistener.h"

JavaListener::~JavaListener() {

}

JavaListener::JavaListener(_JavaVM *vm, _JNIEnv *env, jobject obj) {
    jvm = vm;
    jenv = env;
    jobj = obj;
    jclass clazz = env->GetObjectClass(jobj);
    if (!clazz) {
        return;
    }
    jmId = env->GetMethodID(clazz, "onNext", "(ILjava/lang/String;)V");
}

void JavaListener::onNext(int type, int code, const char *msg) {
    if (type == 0) {
        jvm->AttachCurrentThread(&jenv, NULL);
        jstring jmsg = jenv->NewStringUTF(msg);
        jenv->CallVoidMethod(jobj, jmId, code, jmsg);
        jenv->DeleteLocalRef(jmsg);
        jvm->DetachCurrentThread();
    } else {
        jstring jmsg = jenv->NewStringUTF(msg);
        jenv->CallVoidMethod(jobj, jmId, code, jmsg);
        jenv->DeleteLocalRef(jmsg);
    }
}



//
// Created by EDZ on 2022/4/26.
//

#include "jni.h"
#ifndef RAYMUSIC_JAVALISTENER_H
#define RAYMUSIC_JAVALISTENER_H

class JavaListener {
    JavaVM *jvm;
    JNIEnv *jenv;
    jobject jobj;
    jmethodID jmId;

public:
    JavaListener(JavaVM *vm, JNIEnv *env, jobject obj);

    ~JavaListener();

    /**
     *
     * @param type 1 主线程 0 子线程
     * @param code
     * @param msg
     */
    void onNext(int type, int code, const char *msg);
};


#endif //RAYMUSIC_JAVALISTENER_H

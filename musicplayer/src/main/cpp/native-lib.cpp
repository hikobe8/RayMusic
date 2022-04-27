#include <jni.h>
#include <string>
#include "androidlog.h"
#include "RayFFmpeg.h"

JavaVM *javaVm;
RayFFmpeg *rayFFmpeg;


extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    javaVm = jvm;
    JNIEnv *env;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_4;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1prepare(JNIEnv *env, jobject thiz, jstring url) {
    if (NULL == rayFFmpeg) {
        const char *realUrl = env->GetStringUTFChars(url, 0);
        RayCallJava *rayCallJava = new RayCallJava(javaVm, env, thiz);
        rayFFmpeg = new RayFFmpeg(rayCallJava, realUrl);
    }
    rayFFmpeg->prepare();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_naive_1start(JNIEnv *env, jobject thiz) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->start();
    }
}
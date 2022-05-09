#include <jni.h>
#include <string>
#include "androidlog.h"
#include "RayFFmpeg.h"

JavaVM *javaVm;
RayFFmpeg *rayFFmpeg;
PlayStatus *playStatus;
RayCallJava *rayCallJava;


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
        rayCallJava = new RayCallJava(javaVm, env, thiz);
        playStatus = new PlayStatus();
        rayFFmpeg = new RayFFmpeg(playStatus, rayCallJava, realUrl);
    }
    rayFFmpeg->prepare();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1start(JNIEnv *env, jobject thiz) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->start();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1pause(JNIEnv *env, jobject thiz) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->pause();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1resume(JNIEnv *env, jobject thiz) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1stop(JNIEnv *env, jobject thiz) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->release();
        delete (rayFFmpeg);
        rayFFmpeg = NULL;
        if (NULL != rayCallJava) {
            delete (rayCallJava);
            rayCallJava = NULL;
        }
        if (NULL != playStatus) {
            delete (playStatus);
            playStatus = NULL;
        }
    }
}
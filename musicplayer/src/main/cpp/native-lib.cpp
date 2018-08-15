#include <jni.h>
#include <string>

extern "C"
{
#include <libavformat/avformat.h>
}

#include "RayFFmpeg.h"

JavaVM *javaVM = NULL;
RayCallJava *rayCallJava = NULL;
RayFFmpeg *rayFFmpeg = NULL;
RayPlayStatus* playStatus = NULL;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reversed) {
    javaVM = vm;
    int result = -1;
    JNIEnv *env;
    if (javaVM->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    return JNI_VERSION_1_4;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_ray_player_RayPlayer_native_1prepare(JNIEnv *env, jobject instance, jstring source_) {
    const char *source = env->GetStringUTFChars(source_, 0);
    if (rayFFmpeg == NULL) {
        if (rayCallJava == NULL) {
            rayCallJava = new RayCallJava(javaVM, env, instance);
        }
        rayCallJava->onLoad(MAIN_THEAD, true);
        playStatus = new RayPlayStatus();
        rayFFmpeg = new RayFFmpeg(playStatus, rayCallJava, source);
        rayFFmpeg->prepare();
    }

//    env->ReleaseStringUTFChars(source_, source);
}extern "C"
JNIEXPORT void JNICALL
Java_com_ray_player_RayPlayer_native_1start(JNIEnv *env, jobject instance) {

    if (rayFFmpeg != NULL) {
        rayFFmpeg->start();
    }

}extern "C"
JNIEXPORT void JNICALL
Java_com_ray_player_RayPlayer_native_1pause(JNIEnv *env, jobject instance) {

    if (rayFFmpeg != NULL) {
        rayFFmpeg->pause();
    }

}extern "C"
JNIEXPORT void JNICALL
Java_com_ray_player_RayPlayer_native_1resume(JNIEnv *env, jobject instance) {

    if (rayFFmpeg != NULL) {
        rayFFmpeg->resume();
    }

}extern "C"
JNIEXPORT void JNICALL
Java_com_ray_player_RayPlayer_native_1stop(JNIEnv *env, jobject instance) {

    if (rayFFmpeg != NULL) {
        rayFFmpeg->release();
        delete(rayFFmpeg);
        rayFFmpeg = NULL;

        if (rayCallJava != NULL) {
            delete(rayCallJava);
            rayCallJava = NULL;
        }

        if (playStatus != NULL) {
            delete(playStatus);
            playStatus = NULL;
        }

    }



}
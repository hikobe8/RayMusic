#include <jni.h>
#include <string>
#include "androidlog.h"
#include "RayFFmpeg.h"

JavaVM *javaVm;
RayFFmpeg *rayFFmpeg;
PlayStatus *playStatus;
RayCallJava *rayCallJava;
pthread_t releaseThread;
//是否正在退出，避免重复进行退出操作
bool exiting = false;


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
    const char *realUrl = env->GetStringUTFChars(url, 0);
    if (NULL == rayFFmpeg) {
        if (NULL == rayCallJava) {
            rayCallJava = new RayCallJava(javaVm, env, &thiz);
        }
        rayCallJava->onCallLoading(MAIN_THREAD, true);
        playStatus = new PlayStatus();
        rayFFmpeg = new RayFFmpeg(playStatus, rayCallJava, realUrl);
        rayFFmpeg->prepare();
    }
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

void *releaseRunnable(void *) {
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
    exiting = false;
    pthread_exit(&releaseThread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1stop(JNIEnv *env, jobject thiz) {
    if (exiting)
        return;
    exiting = true;
    if (NULL != rayFFmpeg) {
        pthread_create(&releaseThread, NULL, releaseRunnable, NULL);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1seek(JNIEnv *env, jobject thiz, jint seconds) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->seek(seconds);
    }
}
#include <jni.h>
#include <string>
#include "androidlog.h"
#include "RayFFmpeg.h"

JavaVM *javaVm;
RayFFmpeg *rayFFmpeg = NULL;
PlayStatus *playStatus = NULL;
RayCallJava *rayCallJava = NULL;
pthread_t releaseThread;
pthread_t startThread;
//是否正在退出，避免重复进行退出操作
bool exiting = false;
jmethodID jmidCallPlayNext = NULL;
jobject _jboj = NULL;


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

void *startRunnable(void *data) {
    RayFFmpeg *fFmpeg = (RayFFmpeg *) data;
    fFmpeg->start();
    pthread_exit(&startThread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1start(JNIEnv *env, jobject thiz) {
    if (NULL != rayFFmpeg) {
        pthread_create(&startThread, NULL, startRunnable, rayFFmpeg);
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

void *releaseRunnable(void *data) {
    exiting = true;
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
    JNIEnv *jenv;
    if (javaVm->AttachCurrentThread(&jenv, 0) == JNI_OK) {
        if (NULL != _jboj && NULL != jmidCallPlayNext)
            jenv->CallVoidMethod(_jboj, jmidCallPlayNext);
    }
    javaVm->DetachCurrentThread();
    pthread_exit(&releaseThread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1stop(JNIEnv *env, jobject thiz) {
    if (exiting)
        return;
    if (NULL != rayFFmpeg) {
        jclass clz = env->GetObjectClass(thiz);
        jmidCallPlayNext = env->GetMethodID(clz, "onPlayNextFromNative", "()V");
        _jboj = env->NewGlobalRef(thiz);
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
extern "C"
JNIEXPORT jint JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1duration(JNIEnv *env, jobject thiz) {
    if (NULL != rayFFmpeg) {
        return rayFFmpeg->getDuration();
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1setVolume(JNIEnv *env, jobject thiz, jint volume) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->setVolume(volume);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1setChannel(JNIEnv *env, jobject thiz, jint mode) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->setChannel(mode);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1setSpeed(JNIEnv *env, jobject thiz, jfloat speed) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->setSpeed(speed);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ray_musicplayer_RayPlayer_native_1setPitch(JNIEnv *env, jobject thiz, jfloat pitch) {
    if (NULL != rayFFmpeg) {
        rayFFmpeg->setPitch(pitch);
    }
}
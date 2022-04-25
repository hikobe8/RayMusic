#include <jni.h>
#include <string>
#include "pthread.h"
#include "androidlog.h"

pthread_t thread;

void *threadRunnable(void *) {
    LOGI("pthread running!")
    pthread_exit(&thread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_cppthread_PThreadDemo_createNormalThread(JNIEnv *env, jobject thiz) {
    pthread_create(&thread, NULL, threadRunnable, NULL);
}
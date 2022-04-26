#include <jni.h>
#include <string>
#include "pthread.h"
#include "androidlog.h"
#include "unistd.h"
#include "queue"

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

//生产者线程
pthread_t producerThread;
//消费者线程
pthread_t consumerThread;
//线程锁
pthread_mutex_t mutex;
//条件变量，用于线程等待
pthread_cond_t cond;

//产品队列
std::queue<int> products;

void *producerCallback(void *) {
    while (1) {
        pthread_mutex_lock(&mutex);
        products.push(1);
        LOGI("生产者线程生产1个产品，当前产品总数量为%d", products.size())
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }
    pthread_exit(&producerThread);
}

void *consumerCallback(void *) {
    while (1) {
        pthread_mutex_lock(&mutex);
        if (products.size() > 0) {
            products.pop();
            LOGI("消费者线程消费1个产品，当前产品总数量为%d", products.size())
        } else {
            pthread_cond_wait(&cond, &mutex);
            LOGI("没有产品可消费，等待生产中...")
        }
        pthread_mutex_unlock(&mutex);
        usleep(500 * 1000);
    }
    pthread_exit(&consumerThread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_cppthread_PThreadDemo_producerConsumerModel(JNIEnv *env, jobject thiz) {
    for (int i = 0; i < 10; ++i) {
        products.push(i);
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&producerThread, NULL, producerCallback, NULL);
    pthread_create(&consumerThread, NULL, consumerCallback, NULL);
}

//获取JavaVM
JavaVM *jvm;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *unused) {
    jvm = vm;
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

//C++调用Java方法 主线程

#include "javalistener.h"

JavaListener *javaListener;

extern "C"
JNIEXPORT void JNICALL
Java_com_ray_cppthread_PThreadDemo_cppCallJavaMainThread(JNIEnv *env, jobject thiz) {
    jobject gobj = env->NewGlobalRef(thiz);
    javaListener = new JavaListener(jvm, env, gobj);
    javaListener->onNext(1, 0, "C++在主线程回调Java方法");
    env->DeleteGlobalRef(gobj);
}
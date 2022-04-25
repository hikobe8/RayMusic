#include <jni.h>
#include <string>
#include "android/log.h"

extern "C" {
#include <libavformat/avformat.h>
}

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"hikobe8",FORMAT,##__VA_ARGS__);

extern "C" JNIEXPORT jstring JNICALL
Java_com_ray_musicplayer_JniTest_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);
    while (c_temp != NULL) {
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                LOGI("[Video]:%s", c_temp->name);
                break;
            case AVMEDIA_TYPE_AUDIO:
                LOGI("[Audio]:%s", c_temp->name);
                break;
            default:
                LOGI("[Other]:%s", c_temp->name);
                break;
        }
        c_temp = c_temp->next;
    }

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
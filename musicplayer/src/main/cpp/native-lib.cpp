#include <jni.h>
#include <string>
extern "C"
{
#include <libavformat/avformat.h>
}

JavaVM * javaVM = NULL;

extern "C" JNIEXPORT jint JNICALL JNI_Onload(JavaVM * vm, void* reversed) {
    javaVM = vm;
    int result = -1;
    JNIEnv *env = NULL;
    if (javaVM->GetEnv((void **)(env), JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    return JNI_VERSION_1_4;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_ray_player_RayPlayer_native_1prepare(JNIEnv *env, jobject instance, jstring source_) {
    const char *source = env->GetStringUTFChars(source_, 0);

    // TODO

    env->ReleaseStringUTFChars(source_, source);
}
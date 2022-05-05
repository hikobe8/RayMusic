//
// Created by EDZ on 2022/5/5.
//

#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <jni.h>
#include "androidlog.h"

SLObjectItf engineObject = NULL;
SLEngineItf engineEngine = NULL;

SLObjectItf outputMixObject = NULL;
SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
static const SLEnvironmentalReverbSettings reverbSettings =
        SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
SLObjectItf pcmPlayerObject = NULL;
SLPlayItf pcmPlayer = NULL;
SLAndroidSimpleBufferQueueItf pcmBufferQueue;
FILE *pcmFile;
void *buffer;
uint8_t *out_buffer;

int getPcmData(void **pcm) {
    int size = 0;
    while (!feof(pcmFile)) {
        size = fread(out_buffer, 1, 44100 * 2 * 2, pcmFile);
        if (NULL == out_buffer) {
            LOGI("read end!")
            break;
        } else {
            LOGI("reading")
        }
        *pcm = out_buffer;
        break;
    }
    return size;
}

void pcmBufferCallback(SLAndroidSimpleBufferQueueItf caller,
                       void *pContext) {
    int size = getPcmData(&buffer);
    if (NULL != buffer) {
        (*pcmBufferQueue)->Enqueue(pcmBufferQueue, buffer, size);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_hikobe8_openslesdemo_MainActivity_play(JNIEnv *env, jobject thiz, jstring path) {
    const char *url = env->GetStringUTFChars(path, 0);
    pcmFile = fopen(url, "r");
    out_buffer = (uint8_t *) (malloc(44100 * 2 * 2));
    //创建引擎对象
    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    //Realize
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    //创建引擎实例对象
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    //创建混音器对象
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    //Realize
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    //创建混音器实例对象
    (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                     &outputMixEnvironmentalReverb);
    //设置参数
    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb,
                                                                      &reverbSettings);

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1,
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};
    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    const SLInterfaceID audioIds[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean audioReqs[1] = {SL_BOOLEAN_TRUE};
    //创建播放器对象
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &audioSrc, &audioSnk,
                                       1, audioIds, audioReqs);
    //realize
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    //创建播放器实例对象
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayer);
    //创建缓冲区对象
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    //缓冲区接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallback, NULL);
    (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_PLAYING);
    pcmBufferCallback(pcmBufferQueue, NULL);
    env->ReleaseStringUTFChars(path, url);
}
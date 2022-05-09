//
// Created by Administrator on 2022/4/28.
//

#ifndef RAYMUSIC_RAYAUDIO_H
#define RAYMUSIC_RAYAUDIO_H

#include "PlayStatus.h"
#include "RayQueue.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "RayCallJava.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

class RayAudio {

public:
    int streamIndex;
    AVCodecParameters *codecParameters;
    PlayStatus *playStatus;
    RayQueue *queue;
    pthread_t threadPlay;
    AVCodecContext *avCodecContext;
    AVPacket *avPacket;
    AVFrame *avFrame;
    int ret = 0;
    uint8_t *buffer = NULL;
    int dataSize = 0;
    int sampleRate = 0;
    bool isLoading = true;
    RayCallJava *callJava;
    int duration;
    AVRational timeBase;
    double nowTime; //当前frame时间
    double clock; //当前播放进度时间
    double lastTime; //用于控制时间进度回调，不用每一帧都回调时间进度


    //OpenSLES
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    const SLEnvironmentalReverbSettings reverbSettings =
            SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayer = NULL;
    SLAndroidSimpleBufferQueueItf pcmBufferQueue;

public:
    RayAudio(int index, AVCodecParameters *codecP, PlayStatus *status, RayCallJava* rayCallJava);

    ~RayAudio();

    void play();

    int resampleAudio();

    void freeAvPacket();

    void freeAvFrame();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sampleRate);

    void pause();

    void resume();
};


#endif //RAYMUSIC_RAYAUDIO_H

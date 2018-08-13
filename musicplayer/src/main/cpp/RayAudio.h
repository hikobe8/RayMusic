//
// Created by Administrator on 2018/8/8.
//

#ifndef RAYMUSIC_RAYAUDIO_H
#define RAYMUSIC_RAYAUDIO_H

#include "RayQueue.h"
#include "RayCallJava.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

class RayAudio {
public:
    RayCallJava* callJava;
    int streamIndex = -1;
    AVCodecParameters *codecpar = NULL;
    AVCodecContext *avCodecContext = NULL;
    RayQueue *queuePacket = NULL;
    pthread_t play_thread;
    RayPlayStatus *playStatus = NULL;
    AVPacket * avPacket = NULL;
    AVFrame * avFrame = NULL;
    uint8_t * buffer = NULL;
    int ret = 0;
    int data_size = 0;
    int sample_rate = 0;

    SLObjectItf slEngineObjectItf = NULL;
    SLEngineItf slEngineItf = NULL;
    SLObjectItf outputObjectItf = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLObjectItf playerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLVolumeItf pcmVolumeItf = NULL;
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

public:
    RayAudio(RayCallJava* callJava, RayPlayStatus *playStatus, int sample_rate);

    ~RayAudio();

    void play();

    void pause();

    void resume();

    void stop();

    int resampleAudio();

    void initOpenSLES();

    int getSampleRateForOpenSLES(int sample_rate);
};


#endif //RAYMUSIC_RAYAUDIO_H

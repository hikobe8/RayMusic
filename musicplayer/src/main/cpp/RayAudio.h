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
    RayQueue *packetQueue = NULL;
    pthread_t play_thread;
    RayPlayStatus *playStatus = NULL;
    AVPacket * avPacket = NULL;
    AVFrame * avFrame = NULL;
    uint8_t * buffer = NULL;
    int ret = 0;
    int data_size = 0;
    int sample_rate = 0;
    int duration = 0;
    AVRational time_base;
    double now_time = 0;
    double clock = 0;
    double lastTime = 0;
    int volumePercent = 50;

    SLObjectItf slEngineObjectItf = NULL;
    SLEngineItf slEngineItf = NULL;
    SLObjectItf outputObjectItf = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLVolumeItf pcmVolumePlay = NULL;
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

public:
    RayAudio(RayCallJava* callJava, RayPlayStatus *playStatus, int sample_rate);

    ~RayAudio();

    void play();

    void pause();

    void resume();

    void stop();

    void release();

    int resampleAudio();

    void initOpenSLES();

    int getSampleRateForOpenSLES(int sample_rate);

    void setVolume(int percent);

};


#endif //RAYMUSIC_RAYAUDIO_H

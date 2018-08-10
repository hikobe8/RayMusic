//
// Created by Administrator on 2018/8/8.
//

#ifndef RAYMUSIC_RAYAUDIO_H
#define RAYMUSIC_RAYAUDIO_H

#include "RayQueue.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include <libswresample/swresample.h>
};

class RayAudio {
public:
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

public:
    RayAudio(RayPlayStatus *playStatus);

    ~RayAudio();

    void play();

    int resampleAudio();
};


#endif //RAYMUSIC_RAYAUDIO_H

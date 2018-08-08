//
// Created by Administrator on 2018/8/7.
//

#ifndef RAYMUSIC_RAYFFMPEG_H
#define RAYMUSIC_RAYFFMPEG_H

#include "RayCallJava.h"
#include "pthread.h"
#include "AndnroidLog.h"
#include "RayAudio.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

class RayFFmpeg {

public:
    RayCallJava *callJava;
    const char *url;
    pthread_t decodeThread;
    AVFormatContext *avFormatContext;
    RayAudio *rayAudio = NULL;

public:
    RayFFmpeg(RayCallJava *rayCallJava, const char *url);

    ~RayFFmpeg();

    void prepare();

    void decodeByFFmepg();

    void start();
};


#endif //RAYMUSIC_RAYFFMPEG_H

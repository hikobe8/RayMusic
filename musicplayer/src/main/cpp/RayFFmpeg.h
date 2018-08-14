//
// Created by Administrator on 2018/8/7.
//

#ifndef RAYMUSIC_RAYFFMPEG_H
#define RAYMUSIC_RAYFFMPEG_H

#include "RayCallJava.h"
#include "pthread.h"
#include "RayAudio.h"
#include "RayPlayStatus.h"
extern "C"
{
#include <libavutil/time.h>
#include "libavformat/avformat.h"
};

class RayFFmpeg {

public:
    RayCallJava *callJava;
    const char *url;
    pthread_t decodeThread;
    AVFormatContext *avFormatContext;
    RayAudio *rayAudio = NULL;
    RayPlayStatus* playStatus;
    pthread_mutex_t init_mutex;
    bool exit;

public:
    RayFFmpeg(RayPlayStatus* playStatus, RayCallJava *rayCallJava, const char *url);

    ~RayFFmpeg();

    void prepare();

    void decodeByFFmepg();

    void start();

    void pause();

    void resume();

    void stop();

    void release();
};


#endif //RAYMUSIC_RAYFFMPEG_H

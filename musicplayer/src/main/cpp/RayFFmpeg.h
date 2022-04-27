//
// Created by EDZ on 2022/4/27.
//

#ifndef RAYMUSIC_RAYFFMPEG_H
#define RAYMUSIC_RAYFFMPEG_H

#include "RayCallJava.h"
#include "pthread.h"
#include "androidlog.h"

extern "C" {
#include "libavformat/avformat.h"
};

class RayFFmpeg {
public:
    RayCallJava *callJava;
    const char *url;
    pthread_t decodeThread;
    AVFormatContext *avFormatContext;
public:
    RayFFmpeg(RayCallJava *rayCallJava, const char *url);

    ~RayFFmpeg();

    void prepare();

    void prepareActual();
};


#endif //RAYMUSIC_RAYFFMPEG_H

//
// Created by EDZ on 2022/4/27.
//

#ifndef RAYMUSIC_RAYFFMPEG_H
#define RAYMUSIC_RAYFFMPEG_H

#include "RayCallJava.h"
#include "pthread.h"
#include "androidlog.h"
#include "RayAudio.h"
#include "PlayStatus.h"

extern "C" {
#include "libavformat/avformat.h"
#include <libavutil/time.h>
}

class RayFFmpeg {
public:
    RayCallJava *callJava = NULL;
    const char *url = NULL;
    pthread_t prepareThread;
    AVFormatContext *avFormatContext = NULL;
    RayAudio *rayAudio = NULL;
    PlayStatus *playStatus = NULL;
    pthread_mutex_t initMutex;
    pthread_t seekThread;
    pthread_mutex_t seekMutex;
    bool exit;
    int64_t seekSeconds;

public:
    RayFFmpeg(PlayStatus *status, RayCallJava *rayCallJava, const char *url);

    ~RayFFmpeg();

    void prepare();

    void prepareActual();

    void start();

    void pause();

    void resume();

    void release();

    void seek(int seconds);

    void seekActual();

};


#endif //RAYMUSIC_RAYFFMPEG_H

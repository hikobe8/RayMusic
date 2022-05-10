//
// Created by EDZ on 2022/4/28.
//

#ifndef RAYMUSIC_RAYQUEUE_H
#define RAYMUSIC_RAYQUEUE_H

#include "queue"
#include "pthread.h"
#include "PlayStatus.h"
#include "androidlog.h"

extern "C" {
#include "libavcodec/avcodec.h"
}

class RayQueue {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    std::queue<AVPacket*> queue;
    PlayStatus* playStatus  = NULL;
public:
    RayQueue(PlayStatus* status);
    ~RayQueue();
    int putAVPacket(AVPacket* avPacket);
    int getAVPacket(AVPacket* avPacket);
    int getQueueSize();
    void clearAVPacket();
};


#endif //RAYMUSIC_RAYQUEUE_H

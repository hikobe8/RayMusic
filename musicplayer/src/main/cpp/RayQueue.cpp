//
// Created by Administrator on 2018/8/8.
//

#include "RayQueue.h"

RayQueue::RayQueue(RayPlayStatus* playStatus) {
    this->playStatus = playStatus;
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
}

void RayQueue::putPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    queuePacket.push(packet);
    if (LOG_DEBUG) {
        LOGD("enqueue an AVPacket, size = %d", queuePacket.size());
    }
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
}

int RayQueue::getPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    while (playStatus != NULL && !playStatus->exit) {
        if (queuePacket.size() > 0) {
            AVPacket *avPacket = queuePacket.front();
            if (av_packet_ref(packet, avPacket) == 0) {
                queuePacket.pop();
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            if(LOG_DEBUG)
            {
                LOGD("从队列里面取出一个AVPacket，还剩下 %d 个", queuePacket.size());
            }
            break;
        } else{
            pthread_cond_wait(&condPacket, &mutexPacket);
        }
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

RayQueue::~RayQueue() {
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

long RayQueue::getSize() {
    long size = 0;
    pthread_mutex_lock(&mutexPacket);
    size = queuePacket.size();
    pthread_mutex_unlock(&mutexPacket);
    return size;
}

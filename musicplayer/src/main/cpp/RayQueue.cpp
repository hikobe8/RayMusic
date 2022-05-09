//
// Created by EDZ on 2022/4/28.
//

#include "RayQueue.h"

RayQueue::~RayQueue() {
    clearAVPacket();
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

RayQueue::RayQueue(PlayStatus *status) {
    playStatus = status;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

int RayQueue::putAVPacket(AVPacket *avPacket) {
    pthread_mutex_lock(&mutex);
    queue.push(avPacket);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int RayQueue::getAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutex);
    while (NULL != playStatus && !playStatus->exit) {
        if (queue.size() > 0) {
            //从队列中取出AVPacket
            AVPacket *avPacket = queue.front();
            if (av_packet_ref(packet, avPacket) == 0) {
                queue.pop();
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                break;
            }
        } else {
            pthread_cond_wait(&cond, &mutex);
        }
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

int RayQueue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutex);
    size = queue.size();
    pthread_mutex_unlock(&mutex);
    return size;
}

void RayQueue::clearAVPacket() {
    pthread_cond_signal(&cond);
    pthread_mutex_lock(&mutex);
    while (!queue.empty()) {
        AVPacket* packet = queue.front();
        queue.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&mutex);
}



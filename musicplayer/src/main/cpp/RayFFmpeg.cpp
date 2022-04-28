//
// Created by EDZ on 2022/4/27.
//

#include <unistd.h>
#include "RayFFmpeg.h"

RayFFmpeg::RayFFmpeg(PlayStatus *status, RayCallJava *rayCallJava, const char *url) {
    playStatus = status;
    callJava = rayCallJava;
    this->url = url;
}

RayFFmpeg::~RayFFmpeg() {

}

void *prepareRunnable(void *data) {
    RayFFmpeg *rayFFmpeg = (RayFFmpeg *) (data);
    rayFFmpeg->prepareActual();
    pthread_exit(&rayFFmpeg->prepareThread);
}

void RayFFmpeg::prepare() {
    pthread_create(&prepareThread, NULL, prepareRunnable, this);
}

void RayFFmpeg::prepareActual() {
    av_register_all();
    avformat_network_init();
    avFormatContext = avformat_alloc_context();
    //打开文件流
    if (avformat_open_input(&avFormatContext, url, NULL, NULL) != 0) {
        LOGE("音频文件打开失败!");
        return;
    }
    //获取文件 stream数组信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("获取文件stream信息失败!");
        return;
    }
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGI("获取到音频流 %d", i)
            rayAudio = new RayAudio(i, avFormatContext->streams[i]->codecpar, playStatus);
        }
    }
    //获取解码器
    AVCodec *dec = avcodec_find_decoder(rayAudio->codecParameters->codec_id);
    if (!dec) {
        LOGE("获取音频解码器失败!");
        return;
    }
    AVCodecContext *decContext = avcodec_alloc_context3(dec);
    if (!decContext) {
        LOGE("avcodec_alloc_context3 failed!");
        return;
    }
    if (avcodec_parameters_to_context(decContext, rayAudio->codecParameters) < 0) {
        LOGE("avcodec_parameters_to_context failed!");
        return;
    }
    if (avcodec_open2(decContext, dec, NULL) != 0) {
        LOGE("open stream failed!");
        return;
    }
    callJava->onCallPrepare(CHILD_THREAD);
}

void *decodeActual(void *data) {
    RayFFmpeg *rayFFmpeg = (RayFFmpeg *) (data);
    if (NULL == rayFFmpeg->rayAudio) {
        LOGE("prepare method not called!")
    } else {
        int count = 0;
        while (1) {
            AVPacket *avPacket = av_packet_alloc();
            if (av_read_frame(rayFFmpeg->avFormatContext, avPacket) == 0) {
                //读取到一帧数据
                if (avPacket->stream_index == rayFFmpeg->rayAudio->streamIndex) {
                    count++;
                    LOGI("解码到第%d帧", count)
                    rayFFmpeg->rayAudio->queue->putAVPacket(avPacket);
                } else {
                    av_packet_free(&avPacket);
                    av_free(avPacket);
                }
            } else {
                LOGI("decode finished!")
                av_packet_free(&avPacket);
                av_free(avPacket);
                //读取完成
                break;
            }
        }
        //模拟出队列
        while (rayFFmpeg->rayAudio->queue->getQueueSize() > 0) {
            AVPacket *packet = av_packet_alloc();
            rayFFmpeg->rayAudio->queue->getAVPacket(packet);
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
        }
        LOGI("出队列完成!")
    }
    pthread_exit(&rayFFmpeg->decodeThread);
}

void RayFFmpeg::start() {
    pthread_create(&decodeThread, NULL, decodeActual, this);
}

//
// Created by EDZ on 2022/4/27.
//

#include "RayFFmpeg.h"

RayFFmpeg::RayFFmpeg(RayCallJava *rayCallJava, const char *url) {
    callJava = rayCallJava;
    this->url = url;
}

RayFFmpeg::~RayFFmpeg() {

}

void *prepareRunnable(void *data) {
    RayFFmpeg *rayFFmpeg = (RayFFmpeg *) (data);
    rayFFmpeg->prepareActual();
    pthread_exit(&rayFFmpeg->decodeThread);
}

void RayFFmpeg::prepare() {
    pthread_create(&decodeThread, NULL, prepareRunnable, this);
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
    AVCodecParameters *codecParameters;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGI("获取到音频流 %d", i)
            codecParameters = avFormatContext->streams[i]->codecpar;
        }
    }
    //获取解码器
    AVCodec *dec = avcodec_find_decoder(codecParameters->codec_id);
    if (!dec) {
        LOGE("获取音频解码器失败!");
        return;
    }
    AVCodecContext *decContext = avcodec_alloc_context3(dec);
    if (!decContext) {
        LOGE("avcodec_alloc_context3 failed!");
        return;
    }
    if (avcodec_parameters_to_context(decContext, codecParameters) < 0) {
        LOGE("avcodec_parameters_to_context failed!");
        return;
    }
    if (avcodec_open2(decContext, dec, NULL) != 0) {
        LOGE("open stream failed!");
        return;
    }
    callJava->onCallPrepare(CHILD_THREAD);
}

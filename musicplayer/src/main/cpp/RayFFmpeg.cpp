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
            rayAudio = new RayAudio(i, avFormatContext->streams[i]->codecpar, playStatus, callJava);
        }
    }
    //获取解码器
    AVCodec *dec = avcodec_find_decoder(rayAudio->codecParameters->codec_id);
    if (!dec) {
        LOGE("获取音频解码器失败!");
        return;
    }
    rayAudio->avCodecContext = avcodec_alloc_context3(dec);
    if (!rayAudio->avCodecContext) {
        LOGE("avcodec_alloc_context3 failed!");
        return;
    }
    if (avcodec_parameters_to_context(rayAudio->avCodecContext, rayAudio->codecParameters) < 0) {
        LOGE("avcodec_parameters_to_context failed!");
        return;
    }
    if (avcodec_open2(rayAudio->avCodecContext, dec, NULL) != 0) {
        LOGE("open stream failed!");
        return;
    }
    callJava->onCallPrepare(CHILD_THREAD);
}

void *decodeActual(void *data) {
    RayFFmpeg *rayFFmpeg = (RayFFmpeg *) (data);
    rayFFmpeg->callJava->onCallLoading(CHILD_THREAD, true);
    if (NULL == rayFFmpeg->rayAudio) {
        LOGE("prepare method not called!")
    } else {
        rayFFmpeg->rayAudio->play();
        while (NULL != rayFFmpeg->playStatus && !rayFFmpeg->playStatus->exit) {
            AVPacket *avPacket = av_packet_alloc();
            if (av_read_frame(rayFFmpeg->avFormatContext, avPacket) == 0) {
                //读取到一帧数据
                if (avPacket->stream_index == rayFFmpeg->rayAudio->streamIndex) {
                    rayFFmpeg->rayAudio->queue->putAVPacket(avPacket);
                } else {
                    av_packet_free(&avPacket);
                    av_free(avPacket);
                }
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
                while (NULL != rayFFmpeg->playStatus && !rayFFmpeg->playStatus->exit) {
                    if (rayFFmpeg->rayAudio->queue->getQueueSize() > 0) {
                        continue;
                    } else {
                        LOGI("decode finished!")
                        rayFFmpeg->playStatus->exit = true;
                        break;
                    }
                }
            }
        }
    }
    pthread_exit(&rayFFmpeg->decodeThread);
}

void RayFFmpeg::start() {
    pthread_create(&decodeThread, NULL, decodeActual, this);
}

void RayFFmpeg::pause() {
    if (NULL != rayAudio) {
        rayAudio->pause();
    }
}

void RayFFmpeg::resume() {
    if (NULL != rayAudio) {
        rayAudio->resume();
    }
}

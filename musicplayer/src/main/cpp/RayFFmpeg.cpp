//
// Created by Administrator on 2018/8/7.
//

#include "RayFFmpeg.h"

RayFFmpeg::RayFFmpeg(RayPlayStatus* playStatus, RayCallJava *rayCallJava, const char *url) {
    this->playStatus = playStatus;
    this->callJava = rayCallJava;
    this->url = url;
    this->avFormatContext = NULL;
}

RayFFmpeg::~RayFFmpeg() {

}

void * decodeRunnable(void *data){
    RayFFmpeg* rayFFmpeg = (RayFFmpeg *)(data);
    rayFFmpeg->decodeByFFmepg();
    pthread_exit(&rayFFmpeg->decodeThread);
}

void RayFFmpeg::prepare() {
    pthread_create(&decodeThread, NULL, decodeRunnable, this);
}

void RayFFmpeg::decodeByFFmepg() {
    av_register_all();
    avformat_network_init();
    LOGD("network initialized!");
    avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, url, NULL, NULL) != 0) {
        if (LOG_DEBUG) {
            LOGE("can't open url : %s", url);
        }
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        if (LOG_DEBUG) {
            LOGE("can't find stream from url : %s", url);
        }
        return;
    }
    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (rayAudio == NULL) {
                rayAudio = new RayAudio(playStatus);
                rayAudio->streamIndex = i;
                rayAudio->codecpar = avFormatContext->streams[i]->codecpar;
            }
        }
    }
    if (rayAudio == NULL) {
        if (LOG_DEBUG) {
            LOGW("no audio stream founded : %s!", url);
        }
        return;
    }
    AVCodec * avCodec = avcodec_find_decoder(rayAudio->codecpar->codec_id);
    if (!avCodec) {
        if (LOG_DEBUG) {
            LOGE("can't find audio decoder!");
        }
        return;
    }
    rayAudio->avCodecContext = avcodec_alloc_context3(avCodec);
    if (rayAudio->avCodecContext == NULL) {
        if (LOG_DEBUG) {
            LOGE("can't alloc new decoderContext !");
        }
        return;
    }
    if (avcodec_parameters_to_context(rayAudio->avCodecContext, rayAudio->codecpar) < 0) {
        if (LOG_DEBUG) {
            LOGE("can't fill decoderContext!");
        }
        return;
    }
    if (avcodec_open2(rayAudio->avCodecContext, avCodec, 0) != 0) {
        if (LOG_DEBUG) {
            LOGE("can't open audio streams: %s!", url);
        }
        return;
    }

    callJava->onCallPrepared(CHILD_THEAD);

}

void RayFFmpeg::start() {
    if (rayAudio == NULL) {
        if (LOG_DEBUG) {
            LOGE("audio not initialized!");
        }
        return;
    }
    rayAudio->play();
    int count = 0;
    while (playStatus != NULL && !playStatus->exit) {
        AVPacket *avPacket = av_packet_alloc();
        if (av_read_frame(avFormatContext, avPacket) == 0) {
            if (avPacket->stream_index == rayAudio->streamIndex) {
                count ++;
                if (LOG_DEBUG) {
                    LOGE("解码第 %d 帧", count);
                }
                rayAudio->queuePacket->putPacket(avPacket);
            } else{
                av_packet_unref(avPacket);
                av_free(avPacket);
            }
        } else {
            av_packet_unref(avPacket);
            av_free(avPacket);
            while (playStatus != NULL && !playStatus->exit) {
                if (rayAudio->queuePacket->getSize() > 0) {
                    continue;
                } else{
                    playStatus->exit = true;
                    break;
                }
            }
        }
    }
    if (LOG_DEBUG) {
        LOGE("解码完成!");
    }

}

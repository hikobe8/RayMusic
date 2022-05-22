//
// Created by EDZ on 2022/4/27.
//
#include "RayFFmpeg.h"

RayFFmpeg::RayFFmpeg(PlayStatus *status, RayCallJava *rayCallJava, const char *url) {
    playStatus = status;
    callJava = rayCallJava;
    this->url = url;
    pthread_mutex_init(&initMutex, NULL);
}

RayFFmpeg::~RayFFmpeg() {
    pthread_mutex_destroy(&initMutex);
}

void *prepareRunnable(void *data) {
    RayFFmpeg *rayFFmpeg = (RayFFmpeg *) (data);
    rayFFmpeg->prepareActual();
    pthread_exit(&rayFFmpeg->prepareThread);
}

void RayFFmpeg::prepare() {
    pthread_create(&prepareThread, NULL, prepareRunnable, this);
}

int interruptCallback(void *ctx) {
    RayFFmpeg *context = (RayFFmpeg *) ctx;
    if (context->playStatus->exit) {
        return AVERROR_EOF;
    }
    return 0;
}

void RayFFmpeg::prepareActual() {
    pthread_mutex_lock(&initMutex);
    av_register_all();
    avformat_network_init();
    avFormatContext = avformat_alloc_context();
    //注册avformat_open_input的中断回调
    avFormatContext->interrupt_callback.callback = interruptCallback;
    avFormatContext->interrupt_callback.opaque = this;
    //打开文件流
    if (avformat_open_input(&avFormatContext, url, NULL, NULL) != 0) {
        LOGE("音频文件打开失败!")
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    //获取文件 stream数组信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("获取文件stream信息失败!")
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (NULL == rayAudio) {
                LOGI("获取到音频流 %d", i)
                rayAudio = new RayAudio(i, avFormatContext->streams[i]->codecpar, playStatus,
                                        callJava);
                rayAudio->duration = avFormatContext->duration / AV_TIME_BASE;
                rayAudio->timeBase = avFormatContext->streams[i]->time_base;
            }
        }
    }
    if (NULL == rayAudio) {
        LOGE("当前文件没有音频流信息!!")
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    //获取解码器
    AVCodec *dec = avcodec_find_decoder(rayAudio->codecParameters->codec_id);
    if (!dec) {
        LOGE("获取音频解码器失败!")
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    rayAudio->avCodecContext = avcodec_alloc_context3(dec);
    if (!rayAudio->avCodecContext) {
        LOGE("avcodec_alloc_context3 failed!")
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    if (avcodec_parameters_to_context(rayAudio->avCodecContext, rayAudio->codecParameters) < 0) {
        LOGE("avcodec_parameters_to_context failed!")
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    if (avcodec_open2(rayAudio->avCodecContext, dec, NULL) != 0) {
        LOGE("open stream failed!")
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    if (NULL != callJava) {
        if (NULL != playStatus && !playStatus->exit) {
            callJava->onCallPrepare(CHILD_THREAD);
        } else {
            exit = true;
        }
    }
    pthread_mutex_unlock(&initMutex);
}

void RayFFmpeg::start() {
    if (NULL == rayAudio) {
        LOGE("prepare method not called!")
        return;
    }
    rayAudio->play();
    while (NULL != playStatus && !playStatus->exit) {
        AVPacket *avPacket = av_packet_alloc();
        if (NULL == avFormatContext) {
            exit = true;
            break;
        }
        if (av_read_frame(avFormatContext, avPacket) == 0) {
            //读取到一帧数据
            if (avPacket->stream_index == rayAudio->streamIndex) {
                rayAudio->queue->putAVPacket(avPacket);
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
            }
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            while (NULL != playStatus && !playStatus->exit) {
                if (rayAudio->queue->getQueueSize() > 0) {
                    continue;
                } else {
                    playStatus->exit = true;
                    break;
                }
            }
        }
    }
    LOGI("decode finished!")
    exit = true;
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

void RayFFmpeg::release() {
    LOGI("开始释放ffmpeg")
    if (playStatus->exit) {
        return;
    }
    LOGI("开始释放ffmpeg2")
    playStatus->exit = true;
    pthread_mutex_lock(&initMutex);
    int sleepCount = 0;
    while (!exit) {
        if (sleepCount > 1000) {
            exit = true;
        }
        LOGI("wait ffmpeg  exit %d", sleepCount)
        sleepCount++;
        av_usleep(1000 * 10);
    }
    //释放Audio
    LOGI("释放Audio")
    if (NULL != rayAudio) {
        rayAudio->release();
        delete (rayAudio);
        rayAudio = NULL;
    }
    LOGI("释放 AVFormatContext")
    if (NULL != avFormatContext) {
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        avFormatContext = NULL;
    }
    if (NULL != callJava) {
        callJava = NULL;
    }
    if (NULL != playStatus) {
        playStatus = NULL;
    }
    pthread_mutex_unlock(&initMutex);
}


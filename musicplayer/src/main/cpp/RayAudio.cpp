//
// Created by Administrator on 2022/4/28.
//

#include "RayAudio.h"

RayAudio::RayAudio(int index, AVCodecParameters *codecP, PlayStatus *status,
                   RayCallJava *rayCallJava) {
    streamIndex = index;
    codecParameters = codecP;
    playStatus = status;
    queue = new RayQueue(playStatus);
    sampleRate = codecP->sample_rate;
    buffer = (uint8_t *) (av_malloc(sampleRate * 2 * 2));
    callJava = rayCallJava;
}

RayAudio::~RayAudio() {

}

void *playRunnable(void *data) {
    RayAudio *audio = (RayAudio *) data;
    audio->initOpenSLES();
    pthread_exit(&audio->threadPlay);
}

void RayAudio::play() {
    pthread_create(&threadPlay, NULL, playRunnable, this);
}

int RayAudio::resampleAudio() {
    while (NULL != playStatus && !playStatus->exit) {
        if (queue->getQueueSize() == 0) {
            //当前音频流队列没有数据，回调应用层为loading状态
            if (!isLoading) {
                isLoading = true;
                callJava->onCallLoading(CHILD_THREAD, true);
            }
        } else {
            if (isLoading) {
                isLoading = false;
                callJava->onCallLoading(CHILD_THREAD, false);
            }
        }
        avPacket = av_packet_alloc();
        if (queue->getAVPacket(avPacket) != 0) {
            freeAvPacket();
            continue;
        }
        //把AvPacket传入解码器
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            freeAvPacket();
            continue;
        }
        avFrame = av_frame_alloc();
        //从解码器中取出解码的数据
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == 0) {
            if (avFrame->channels > 0 && avFrame->channel_layout == 0) {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }
            //创建重采样上下文环境
            SwrContext *swrContext;
            swrContext = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    (AVSampleFormat) avFrame->format,
                    avFrame->sample_rate,
                    NULL,
                    NULL
            );
            if (!swrContext || swr_init(swrContext) < 0) {
                freeAvPacket();
                freeAvFrame();
                swr_free(&swrContext);
                continue;
            }
            //开始重采样
            int nb = swr_convert(
                    swrContext,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **) avFrame->data,
                    avFrame->nb_samples
            );
            int outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            //获取一个frame大小
            dataSize = nb * outChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            nowTime = avFrame->pts * av_q2d(timeBase);
            if (nowTime < clock) { //pts 有可能为0
                nowTime = clock;
            }
            clock = nowTime;
            freeAvPacket();
            freeAvFrame();
            swr_free(&swrContext);
            break;
        } else {
            freeAvPacket();
            freeAvFrame();
            continue;
        }
    }
    return dataSize;
}

void RayAudio::freeAvFrame() {
    av_frame_free(&avFrame);
    av_free(avFrame);
    avFrame = NULL;
}

void RayAudio::freeAvPacket() {
    av_packet_free(&avPacket);
    av_free(avPacket);
    avPacket = NULL;
}

void pcmBufferCallback(SLAndroidSimpleBufferQueueItf caller,
                       void *pContext) {
    RayAudio *rayAudio = (RayAudio *) pContext;
    int size = rayAudio->resampleAudio();
    if (size > 0) {
        rayAudio->clock += size / ((double) (rayAudio->sampleRate * 2 * 2));
        if (rayAudio->clock - rayAudio->lastTime >= 0.1) { //0.1秒回调一次
            rayAudio->lastTime = rayAudio->clock;
            rayAudio->callJava->onCallProgressChange(CHILD_THREAD, rayAudio->clock,
                                                     rayAudio->duration);
        }
        (*rayAudio->pcmBufferQueue)->Enqueue(rayAudio->pcmBufferQueue, rayAudio->buffer, size);
    }
}

void RayAudio::initOpenSLES() {
    //创建引擎对象
    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    //Realize
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    //创建引擎实例对象
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    //创建混音器对象
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    //Realize
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    //创建混音器实例对象
    (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                     &outputMixEnvironmentalReverb);
    //设置参数
    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb,
                                                                      &reverbSettings);

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2,
                                   (SLuint32) getCurrentSampleRateForOpensles(sampleRate),
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};
    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    const SLInterfaceID audioIds[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_MUTESOLO};
    const SLboolean audioReqs[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    //创建播放器对象
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &audioSrc, &audioSnk,
                                       3, audioIds, audioReqs);
    //realize
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    //创建播放器实例对象
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayer);
    //创建缓冲区对象
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    //缓冲区接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallback, this);
    (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_PLAYING);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmVolumeObject);
    setVolume(volumePercent);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_MUTESOLO, &pcmMuteSolo);
    setChannel(channelMode);
    pcmBufferCallback(pcmBufferQueue, this);
}

int RayAudio::getCurrentSampleRateForOpensles(int sampleRate) {
    int rate = 0;
    switch (sampleRate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void RayAudio::pause() {
    if (NULL != pcmPlayer) {
        (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_PAUSED);
        callJava->onCallPause(MAIN_THREAD);
    }
}

void RayAudio::resume() {
    if (NULL != pcmPlayer) {
        (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_PLAYING);
        callJava->onCallResume(MAIN_THREAD);
    }
}

void RayAudio::stop() {
    if (NULL != pcmPlayer) {
        (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_STOPPED);
    }

}

void RayAudio::release() {
    //删除AVPacket队列，释放内存
    if (NULL != queue) {
        delete (queue);
        queue = NULL;
    }
    //清理PlayerObject, Player, PCM buffer queue对应的内存
    if (NULL != pcmPlayerObject) {
        (*pcmPlayerObject)->Destroy(pcmPlayerObject);
        pcmPlayerObject = NULL;
        pcmPlayer = NULL;
        pcmBufferQueue = NULL;
    }

    //清理混音器对应的内存
    if (NULL != outputMixObject) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }
    //清理引擎对应的内存
    if (NULL != engineObject) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
    if (NULL != buffer) {
        free(buffer);
        buffer = NULL;
    }
    if (NULL != avCodecContext) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }
    if (NULL != callJava) {
        callJava = NULL;
    }
    if (NULL != playStatus) {
        playStatus = NULL;
    }
}

void RayAudio::setVolume(int percent) {
    volumePercent = percent;
    if (pcmVolumeObject != NULL) {
        if (percent > 30) {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -20);
        } else if (percent > 25) {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -22);
        } else if (percent > 20) {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -25);
        } else if (percent > 15) {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -28);
        } else if (percent > 10) {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -30);
        } else if (percent > 5) {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -34);
        } else if (percent > 3) {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -37);
        } else if (percent > 0) {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -40);
        } else {
            (*pcmVolumeObject)->SetVolumeLevel(pcmVolumeObject, (100 - percent) * -100);
        }
    }
}

void RayAudio::setChannel(int mode) {
    channelMode = mode;
    if (NULL == pcmMuteSolo)
        return;
    if (mode == 0) {
        //左声道
        (*pcmMuteSolo)->SetChannelSolo(pcmMuteSolo, 0, true);
        (*pcmMuteSolo)->SetChannelSolo(pcmMuteSolo, 1, false);
    } else if (mode == 1) {
        //右声道
        (*pcmMuteSolo)->SetChannelSolo(pcmMuteSolo, 0, false);
        (*pcmMuteSolo)->SetChannelSolo(pcmMuteSolo, 1, true);
    } else {
        //立体声
        (*pcmMuteSolo)->SetChannelSolo(pcmMuteSolo, 0, false);
        (*pcmMuteSolo)->SetChannelSolo(pcmMuteSolo, 1, false);
    }
}

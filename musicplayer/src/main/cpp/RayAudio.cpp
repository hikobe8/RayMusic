//
// Created by Administrator on 2018/8/8.
//

#include "RayAudio.h"

RayAudio::RayAudio(RayCallJava* callJava, RayPlayStatus *playStatus, int sample_rate) {
    this->callJava = callJava;
    this->playStatus = playStatus;
    this->sample_rate = sample_rate;
    this->queuePacket = new RayQueue(playStatus);
    buffer = (uint8_t *) (av_malloc(sample_rate * 2 * 2));
}

RayAudio::~RayAudio() {

}

void *resampleRunnable(void *data) {
    //!!!fatal error :   RayAudio *audio = (RayAudio *) (&data);
    RayAudio *audio = (RayAudio *) (data);
    audio->initOpenSLES();
    pthread_exit(&audio->play_thread);
}


void RayAudio::play() {
    pthread_create(&play_thread, NULL, resampleRunnable, this);
}

int RayAudio::resampleAudio() {
    while (playStatus != NULL && !playStatus->exit) {
        if (queuePacket->getSize() == 0) {
            //正在加载
            if (!playStatus->isLoading) {
                playStatus->isLoading = true;
                callJava->onLoad(CHILD_THEAD, true);
            }
            continue;
        } else {
            //加载完成
            if (playStatus->isLoading) {
                playStatus->isLoading = false;
                callJava->onLoad(CHILD_THEAD, false);
            }
        }

        avPacket = av_packet_alloc();
        if (queuePacket->getPacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == 0) {
            if (avFrame->channels > 0 && avFrame->channel_layout == 0) {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }

            SwrContext *swr_ctx;
            swr_ctx = swr_alloc_set_opts(NULL,
                                         AV_CH_LAYOUT_STEREO,
                                         AV_SAMPLE_FMT_S16,
                                         avFrame->sample_rate,
                                         avFrame->channel_layout,
                                         (AVSampleFormat) (avFrame->format),
                                         avFrame->sample_rate,
                                         NULL, NULL);

            if (!swr_ctx || swr_init(swr_ctx) < 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                swr_free(&swr_ctx);
                continue;
            }
            int nb = swr_convert(swr_ctx,
                                 &buffer,
                                 avFrame->nb_samples,
                                 (const uint8_t **) (avFrame->data),
                                 avFrame->nb_samples
            );

            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swr_ctx);
            break;
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;
        }
    }
    return data_size;
}

void pcmBufferCallback(SLAndroidSimpleBufferQueueItf caller,
                       void *pContext) {
    RayAudio *rayAudio = (RayAudio *) (pContext);
    int dataSize = rayAudio->resampleAudio();
    (*rayAudio->pcmBufferQueue)->Enqueue(rayAudio->pcmBufferQueue, rayAudio->buffer, dataSize);
}

void RayAudio::initOpenSLES() {
    //1.创建引擎对象
    slCreateEngine(&slEngineObjectItf, 0, 0, 0, 0, 0);
    (*slEngineObjectItf)->Realize(slEngineObjectItf, SL_BOOLEAN_FALSE);
    (*slEngineObjectItf)->GetInterface(slEngineObjectItf, SL_IID_ENGINE, &slEngineItf);

    const SLInterfaceID pInterfaceIds[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean pInterfaceRequireds[1] = {SL_BOOLEAN_FALSE};
    //2.创建混音器
    (*slEngineItf)->CreateOutputMix(slEngineItf, &outputObjectItf, 1, pInterfaceIds,
                                    pInterfaceRequireds);
    (*outputObjectItf)->Realize(outputObjectItf, SL_BOOLEAN_FALSE);
    (*outputObjectItf)->GetInterface(outputObjectItf, SL_IID_ENVIRONMENTALREVERB,
                                     &outputMixEnvironmentalReverb);

    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb,
                                                                      &reverbSettings);
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputObjectItf};
    //3.创建播放器
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//数据格式
            2,//声道格式
            (SLuint32)(getSampleRateForOpenSLES(sample_rate)), //采样率
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource dataSource = {&android_queue, &pcm};
    SLDataSink dataSink = {&outputMix, NULL};
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean reqs[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*slEngineItf)->CreateAudioPlayer(slEngineItf, &playerObject, &dataSource, &dataSink, 3, ids,
                                      reqs);
    //初始化播放器
    (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    //得到接口
    (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &pcmPlayerPlay);

    //4创建缓冲区和回调函数
    (*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    //缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallback, this);
    //获取音量接口
    (*playerObject)->GetInterface(playerObject, SL_IID_VOLUME, &pcmVolumeItf);
    //设置播放状态
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);

    pcmBufferCallback(pcmBufferQueue, this);
}

int RayAudio::getSampleRateForOpenSLES(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025000:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 16000000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 22050000:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100000:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200000:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
            break;
    }
    return rate;
}

void RayAudio::pause() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void RayAudio::resume() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void RayAudio::stop() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
}

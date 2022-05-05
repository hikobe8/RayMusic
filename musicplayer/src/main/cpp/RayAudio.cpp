//
// Created by Administrator on 2022/4/28.
//

#include "RayAudio.h"

RayAudio::RayAudio(int index, AVCodecParameters *codecP, PlayStatus *status) {
    streamIndex = index;
    codecParameters = codecP;
    playStatus = status;
    queue = new RayQueue(playStatus);
    buffer = (uint8_t *) (av_malloc(44100 * 2 * 2));
}

RayAudio::~RayAudio() {

}

void *playRunnable(void *data) {
    RayAudio *audio = (RayAudio *) data;
    audio->resampleAudio();
    pthread_exit(&audio->threadPlay);
}

void RayAudio::play() {
    pthread_create(&threadPlay, NULL, playRunnable, this);
}

FILE *outFile = fopen("/sdcard/test.pcm", "w");

int RayAudio::resampleAudio() {
    while (NULL != playStatus && !playStatus->exit) {
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
            fwrite(buffer, dataSize, 1, outFile);
            LOGI("frame data size is %d", dataSize)
            freeAvPacket();
            freeAvFrame();
            swr_free(&swrContext);
        } else {
            freeAvPacket();
            freeAvFrame();
            continue;
        }
    }
    fclose(outFile);
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

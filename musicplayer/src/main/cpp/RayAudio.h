//
// Created by Administrator on 2022/4/28.
//

#ifndef RAYMUSIC_RAYAUDIO_H
#define RAYMUSIC_RAYAUDIO_H

#include "PlayStatus.h"
#include "RayQueue.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "RayCallJava.h"
#include "SoundTouch.h"

using namespace soundtouch;

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

class RayAudio {

public:
    int streamIndex = -1;
    AVCodecParameters *codecParameters = NULL;
    PlayStatus *playStatus = NULL;
    RayQueue *queue = NULL;
    pthread_t threadPlay;
    AVCodecContext *avCodecContext = NULL;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = 0;
    uint8_t *buffer = NULL;
    int dataSize = 0;
    int sampleRate = 0;
    bool isLoading = true;
    RayCallJava *callJava = NULL;
    int duration = 0;
    AVRational timeBase;
    double nowTime = 0.0; //当前frame时间
    double clock = 0.0; //当前播放进度时间
    double lastTime = 0.0; //用于控制时间进度回调，不用每一帧都回调时间进度
    int volumePercent = 100;
    int channelMode = 1; //默认左声道

    //OpenSLES
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    const SLEnvironmentalReverbSettings reverbSettings =
            SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayer = NULL;
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;
    SLVolumeItf pcmVolumeObject = NULL;
    SLMuteSoloItf pcmMuteSolo = NULL;

    //soundtouch
    SoundTouch* soundTouch = NULL;
    SAMPLETYPE* sampleBuffer = NULL;
    uint8_t * out_buffer = NULL;
    bool finished = true;
    int nb = 0;
    int num = 0;
    //速度
    float speed = 1.0f;
    //音调
    float pitch = 1.0f;

public:
    RayAudio(int index, AVCodecParameters *codecP, PlayStatus *status, RayCallJava *rayCallJava);

    ~RayAudio();

    void play();

    int resampleAudio(void ** pcmBuffer);

    void freeAvPacket();

    void freeAvFrame();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sampleRate);

    void pause();

    void resume();

    void stop();

    void release();

    void setVolume(int percent);

    void setChannel(int mode);

    int getSoundTouchData();

    void setPitch(float pitch);

    void setSpeed(float speed);

};


#endif //RAYMUSIC_RAYAUDIO_H

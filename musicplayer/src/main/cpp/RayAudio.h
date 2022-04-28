//
// Created by Administrator on 2022/4/28.
//

#ifndef RAYMUSIC_RAYAUDIO_H
#define RAYMUSIC_RAYAUDIO_H

#include "PlayStatus.h"
#include "RayQueue.h"

extern "C"{
#include "libavformat//avformat.h"
};

class RayAudio {

public:
    int streamIndex;
    AVCodecParameters* codecParameters;
    PlayStatus* playStatus;
    RayQueue* queue;

public:
    RayAudio(int index, AVCodecParameters* codecP, PlayStatus* status);
    ~RayAudio();
};


#endif //RAYMUSIC_RAYAUDIO_H

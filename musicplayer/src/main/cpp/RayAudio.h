//
// Created by Administrator on 2022/4/28.
//

#ifndef RAYMUSIC_RAYAUDIO_H
#define RAYMUSIC_RAYAUDIO_H

extern "C"{
#include "libavformat//avformat.h"
};

class RayAudio {

public:
    int streamIndex;
    AVCodecParameters* codecParameters;

public:
    RayAudio(int index, AVCodecParameters* codecP);
    ~RayAudio();
};


#endif //RAYMUSIC_RAYAUDIO_H

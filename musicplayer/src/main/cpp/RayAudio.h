//
// Created by Administrator on 2018/8/8.
//

#ifndef RAYMUSIC_RAYAUDIO_H
#define RAYMUSIC_RAYAUDIO_H

extern "C" {
#include "libavformat/avformat.h"
};

class RayAudio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecpar = NULL;
    AVCodecContext *avCodecContext = NULL;
public:
    RayAudio();
    ~RayAudio();
};


#endif //RAYMUSIC_RAYAUDIO_H

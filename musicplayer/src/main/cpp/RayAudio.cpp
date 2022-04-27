//
// Created by Administrator on 2022/4/28.
//

#include "RayAudio.h"

RayAudio::RayAudio(int index, AVCodecParameters *codecP) {
    streamIndex = index;
    codecParameters = codecP;
}

RayAudio::~RayAudio() {

}

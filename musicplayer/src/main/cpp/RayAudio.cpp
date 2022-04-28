//
// Created by Administrator on 2022/4/28.
//

#include "RayAudio.h"

RayAudio::RayAudio(int index, AVCodecParameters *codecP,  PlayStatus* status) {
    streamIndex = index;
    codecParameters = codecP;
    playStatus = status;
    queue = new RayQueue(playStatus);
}

RayAudio::~RayAudio() {

}

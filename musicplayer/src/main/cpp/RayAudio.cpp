//
// Created by Administrator on 2018/8/8.
//

#include "RayAudio.h"

RayAudio::RayAudio(RayPlayStatus* playStatus) {
    this->queuePacket = new RayQueue(playStatus);
}

RayAudio::~RayAudio() {

}

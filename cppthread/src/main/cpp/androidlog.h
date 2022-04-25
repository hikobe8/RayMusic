//
// Created by Administrator on 2022/4/25.
//

#include "android/log.h"
#ifndef RAYMUSIC_ANDROIDLOG_H
#define RAYMUSIC_ANDROIDLOG_H

#endif //RAYMUSIC_ANDROIDLOG_H

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"hikobe8",FORMAT,##__VA_ARGS__);

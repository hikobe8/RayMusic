//
// Created by EDZ on 2022/4/27.
//

#ifndef RAYMUSIC_ANDROIDLOG_H
#define RAYMUSIC_ANDROIDLOG_H

#endif //RAYMUSIC_ANDROIDLOG_H

#define DEBUG true

#include "android/log.h"

#if(DEBUG)
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"hikobe8",FORMAT,##__VA_ARGS__);
#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"hikobe8",FORMAT,##__VA_ARGS__);
#define LOGW(FORMAT, ...) __android_log_print(ANDROID_LOG_WARN,"hikobe8",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"hikobe8",FORMAT,##__VA_ARGS__);
#else
#define LOGI(FORMAT, ...)//do nothing
#define LOGD(FORMAT, ...)//do nothing
#define LOGW(FORMAT, ...)//do nothing
#define LOGE(FORMAT, ...)//do nothing
#endif

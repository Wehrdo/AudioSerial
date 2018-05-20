//
// Created by dawehr on 11/24/2016.
//

#ifndef ANDROID_COMMON_H
#define ANDROID_COMMON_H

#include <android/log.h>
#define LOG_TAG "AudioSerial-native"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))

#endif //ANDROID_COMMON_H

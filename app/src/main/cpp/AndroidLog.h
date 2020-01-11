//
// Created by 天涯路 on 2020-01-11.
//

#ifndef NEJNILOGINMD5ENCRYPT_ANDROIDLOG_H
#define NEJNILOGINMD5ENCRYPT_ANDROIDLOG_H

#include <android/log.h>

#define TAG "STY"
//#define LOGE2(...) __android_log_print(ANDROID_LOG_ERROR, TAG, ##__VA_ARGS__) //error: format string is not a string literal (potentially insecure)
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR, TAG, FORMAT, ##__VA_ARGS__)
#define LOGV(FORMAT,...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, FORMAT, ##__VA_ARGS__)
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO, TAG, FORMAT, ##__VA_ARGS__)
#define LOGW(FORMAT,...) __android_log_print(ANDROID_LOG_WARN, TAG, FORMAT, ##__VA_ARGS__)
#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG, TAG, FORMAT, ##__VA_ARGS__)

#endif //NEJNILOGINMD5ENCRYPT_ANDROIDLOG_H

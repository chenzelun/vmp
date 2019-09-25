//
// Created by ChenDalunlun on 2019/8/27.
//

#ifndef SHELLAPPLICATION_UTIL_H
#define SHELLAPPLICATION_UTIL_H

#include <jni.h>
#include "common/Dalvik.h"
#include <string>
#include <android/log.h>

using namespace std;

#define BASE_10 10

#define SHELL_LOG

#if defined(SHELL_LOG)
#define LOG_D(...) __android_log_print(ANDROID_LOG_DEBUG,__FUNCTION__,__VA_ARGS__) // 定义LOGD类型
#define LOG_E(...) __android_log_print(ANDROID_LOG_ERROR,__FUNCTION__,__VA_ARGS__) // 定义LOGE类型
#else
#define LOG_D(...)
#define LOG_E(...)
#endif

jobject getAppContext(JNIEnv *env);

jobject getAssetsManager(JNIEnv *env);

const string getCUP_ABI();

jobject getClassLoader(JNIEnv *env);

bool HookNativeInline(const char *soPath, const char *signature, void *my_func, void **ori_func);

bool HookNativeInlineAnonymous(const char *soPath, uint64_t addr, void *my_func, void **ori_func);

bool
HookJava(JNIEnv *env, const char *clazzPath, const char *methodName, const char *methodSignature,
         const void *my_func, jmethodID *ori_func);

string getSystemGetProperty(JNIEnv *env, const char *propertyName);

bool isArtVm(JNIEnv *env);

void debugMaps();

const char* getLinkerPath();

#endif //SHELLAPPLICATION_UTIL_H

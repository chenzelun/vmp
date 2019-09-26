//
// Created by ChenD on 2019/5/20.
//

#include <dlfcn.h>
#include <jni.h>
#include <AndHook.h>
#include <DVMInNeed.h>
#include "Utils.h"

bool HookNativeInline(const char *soPath, const char *signature, void *my_func,
                      void **ori_func) {
    LOGD("start HookNativeInline...");
    auto *lib = AKGetImageByName(soPath);
    if (lib == nullptr) {
        LOGE("can't find so by the path: %s", soPath);
        return false;
    }
    LOGD("lib base address: %p", AKGetBaseAddress(lib));

    auto *symbol = AKFindSymbol(lib, signature);
    if (symbol == nullptr) {
        LOGE("can't find the symbol: %s", signature);
        AKCloseImage(lib);
        return false;
    }
    AKHookFunction(symbol, my_func, ori_func);
    AKCloseImage(lib);
    return true;
}


void GetSymbolByDLOpen(const char *soPath, const char *signature, void **result) {
    LOGD("start GetSymbolByDLOpen...");
    void *so = dlopen(soPath, RTLD_LAZY);
    if (!so) {
        LOGE("can't find so by the path: %s", soPath);
        *result = nullptr;
        return;
    }
    LOGD("found so: %s", soPath);

    void *symbol = dlsym(so, signature);
    if (!symbol) {
        LOGE("can't find symbol: %s in %s", signature, soPath);
        *result = nullptr;
        return;
    }
    LOGD("found the symbol: %s", signature);
    *result = symbol;
}

bool HookJava(JNIEnv *env, const char *clazzPath, const char *methodName,
              const char *methodSignature, const void *my_func, jmethodID *ori_func) {

    LOGD("start HookJava...");
    jclass clazz = (*env).FindClass(clazzPath);
    if (clazz == nullptr) {
        LOGE("can't find the class by the path: %s", clazzPath);
        return false;
    }
    AKJavaHookMethod(env, clazz, methodName, methodSignature, my_func, ori_func);
    (*env).DeleteLocalRef(clazz);
    return true;
}



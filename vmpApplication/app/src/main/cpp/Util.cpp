//
// Created by ChenDalunlun on 2019/8/27.
//

#include "Util.h"

#include <cassert>
#include <dlfcn.h>
#include "Util.h"
#include "include/AndHook.h"

bool HookNativeInline(const char *soPath, const char *signature, void *my_func, void **ori_func) {
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

bool
HookJava(JNIEnv *env, const char *clazzPath, const char *methodName, const char *methodSignature,
         const void *my_func, jmethodID *ori_func) {
    LOGD("start HookJava...");
    // init AndHook Java
    static bool initAndHookJava = true;
    if (initAndHookJava) {
        initAndHookJava = false;
        JavaVM *vm = nullptr;
        (*env).GetJavaVM(&vm);
        AKInitializeOnce(env, vm);
        LOGD("init java hook vm...");
    }
    jclass clazz = (*env).FindClass(clazzPath);
    if (clazz == nullptr) {
        LOGE("can't find the class by the path: %s", clazzPath);
        return false;
    }
    AKJavaHookMethod(env, clazz, methodName, methodSignature, my_func, ori_func);
    (*env).DeleteLocalRef(clazz);
    return true;
}

jobject getAppContext(JNIEnv *env) {
    static jobject appContext = nullptr;
    if (appContext == nullptr) {
        jclass cActivityThread = (*env).FindClass("android/app/ActivityThread");
        jmethodID mCurrentActivityThread = (*env).GetStaticMethodID(
                cActivityThread, "currentActivityThread",
                "()Landroid/app/ActivityThread;");
        jobject oCurrentActivityThread = (*env).CallStaticObjectMethod(
                cActivityThread, mCurrentActivityThread);
        assert(oCurrentActivityThread != nullptr);
        jmethodID mGetApplication = (*env).GetMethodID(
                cActivityThread, "getApplication",
                "()Landroid/app/Application;");
        jobject oCurrentApplication = (*env).CallObjectMethod(oCurrentActivityThread,
                                                              mGetApplication);
        jclass cApplication = (*env).FindClass("android/app/Application");
        jmethodID mGetApplicationContext = (*env).GetMethodID(
                cApplication, "getApplicationContext",
                "()Landroid/content/Context;");
        jobject oContext = (*env).CallObjectMethod(oCurrentApplication, mGetApplicationContext);
        appContext = (*env).NewGlobalRef(oContext);
        LOGD("getAppContext success first....");
        (*env).DeleteLocalRef(cActivityThread);
        (*env).DeleteLocalRef(cApplication);
    }
    return appContext;
}

jobject getAssetsManager(JNIEnv *env) {
    static jobject assetsManager = nullptr;
    if (assetsManager == nullptr) {
        jclass cContextWrapper = (*env).FindClass("android/content/ContextWrapper");
        jmethodID mGetAssets = (*env).GetMethodID(cContextWrapper, "getAssets",
                                                  "()Landroid/content/res/AssetManager;");
        assetsManager = (*env).CallObjectMethod(getAppContext(env), mGetAssets);

        assert(assetsManager != nullptr);
        LOGD("getAssetsManager success first....");
        (*env).DeleteLocalRef(cContextWrapper);
    }
    return assetsManager;
}

const string getCUP_ABI() {
#if defined(__arm__)
#if defined(__ARM_ARCH_7A__)
#if defined(__ARM_NEON__)
#if defined(__ARM_PCS_VFP)
    return "armeabi-v7a/NEON (hard-float)";
#else
    return "armeabi-v7a/NEON";
#endif
#else
#if defined(__ARM_PCS_VFP)
    return "armeabi-v7a (hard-float)";
#else
    return "armeabi-v7a";
#endif
#endif
#else
    return "armeabi";
#endif
#elif defined(__i386__)
    return "x86";
#elif defined(__x86_64__)
    return "x86_64";
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
    return "mips64";
#elif defined(__mips__)
    return "mips";
#elif defined(__aarch64__)
    return "arm64-v8a";
#else
    return "unknown";
#endif
}

jobject getClassLoader(JNIEnv *env) {
    static jobject oClassLoader = nullptr;
    if (oClassLoader == nullptr) {
        jobject oContext = getAppContext(env);
        jclass cContext = (*env).GetObjectClass(oContext);
        jmethodID mGetClassLoader = (*env).GetMethodID(cContext, "getClassLoader",
                                                       "()Ljava/lang/ClassLoader;");
        oClassLoader = (*env).CallObjectMethod(oContext, mGetClassLoader);
        LOGD("getClassLoader first...");
    }
    return oClassLoader;
}

bool isArtVm(JNIEnv *env) {
    static int version = 0;
    if (version == 0) {
        string versionStr = getSystemGetProperty(env, "java.vm.version");
        if (versionStr.empty()) {
            return false;
        }
        LOGD("java.vm.version: %s", versionStr.data());
        version = stoi(versionStr, nullptr, BASE_10);
    }
    return version >= 2;
}

string getSystemGetProperty(JNIEnv *env, const char *propertyName) {
    jclass cSystem = (*env).FindClass("java/lang/System");
    jmethodID mGetProperty = (*env).GetStaticMethodID(cSystem, "getProperty",
                                                      "(Ljava/lang/String;)Ljava/lang/String;");
    jstring propertyNameJStr = (*env).NewStringUTF(propertyName);
    jobject result = (*env).CallStaticObjectMethod(cSystem, mGetProperty, propertyNameJStr);
    if (result == nullptr) {
        return "";
    } else {
        return (*env).GetStringUTFChars(reinterpret_cast<jstring>(result), JNI_FALSE);
    }
}

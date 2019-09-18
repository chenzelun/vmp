//
// Created by ChenDalunlun on 2019/8/27.
//
#include <jni.h>
#include "Util.h"
#include "Config.h"
#include "Dex.h"
#include <android/api-level.h>


// global variable
JNIEnv *env = nullptr;
ConfigFileProxy *gConfigFileProxy = nullptr;
DexFileHelper *gDexFileHelper = nullptr;


void changeTopApplication() {
    LOG_D("start, changeTopApplication()");
    static jobject oApplication = nullptr;
    if (oApplication == nullptr) {
        // config dynamic loading env
        LOG_D("0");
        loadDexFromMemory();
        LOG_D("1");
        // get main thread's object
        jclass cActivityThread = (*env).FindClass("android/app/ActivityThread");
        jmethodID mCurrentActivityThread = (*env).GetStaticMethodID(
                cActivityThread, "currentActivityThread",
                "()Landroid/app/ActivityThread;");
        jobject oCurrentActivityThread = (*env).CallStaticObjectMethod(cActivityThread,
                                                                       mCurrentActivityThread);
        LOG_D("2");
        jstring sAppName = (*env).NewStringUTF(gConfigFileProxy->srcApkApplicationName.data());
        //有值的话调用该Applicaiton
        jfieldID fMBoundApplication = (*env).GetFieldID(cActivityThread, "mBoundApplication",
                                                        "Landroid/app/ActivityThread$AppBindData;");
        jobject oMBoundApplication = (*env).GetObjectField(oCurrentActivityThread,
                                                           fMBoundApplication);
        LOG_D("3");
        jclass cAppBindData = (*env).GetObjectClass(oMBoundApplication);
        jfieldID fInfo = (*env).GetFieldID(cAppBindData, "info", "Landroid/app/LoadedApk;");
        jobject oLoadedApkInfo = (*env).GetObjectField(oMBoundApplication, fInfo);
        LOG_D("4");
        jclass cLoadedApk = (*env).FindClass("android/app/LoadedApk");
        jfieldID fMApplication = (*env).GetFieldID(cLoadedApk, "mApplication",
                                                   "Landroid/app/Application;");
        (*env).SetObjectField(oLoadedApkInfo, fMApplication, nullptr);
        //set 'mApplication = null' in current thread.
        LOG_D("5");
        jfieldID fMInitialApplication = (*env).GetFieldID(cActivityThread, "mInitialApplication",
                                                          "Landroid/app/Application;");
        jobject oMInitialApplication = (*env).GetObjectField(oCurrentActivityThread,
                                                             fMInitialApplication);
        LOG_D("6");
        jfieldID fMAllApplications = (*env).GetFieldID(cActivityThread, "mAllApplications",
                                                       "Ljava/util/ArrayList;");
        jobject oMAllApplications = (*env).GetObjectField(oCurrentActivityThread,
                                                          fMAllApplications);
        LOG_D("7");
        assert(oMAllApplications != nullptr);
        jclass cArrayList = (*env).GetObjectClass(oMAllApplications);
        jmethodID mRemove = (*env).GetMethodID(cArrayList, "remove", "(Ljava/lang/Object;)Z");
        auto isRemove = (*env).CallBooleanMethod(oMAllApplications, mRemove, oMInitialApplication);
        if (isRemove == JNI_TRUE) {
            LOG_D("delete oldApplication    ok...");
        } else {
            LOG_E("delete oldApplication    failed...");
        }
        LOG_D("8");
        jfieldID fMApplicationInfo = (*env).GetFieldID(cLoadedApk, "mApplicationInfo",
                                                       "Landroid/content/pm/ApplicationInfo;");
        jobject oAppInfo0 = (*env).GetObjectField(oLoadedApkInfo, fMApplicationInfo);
        LOG_D("9");
        assert(oAppInfo0 != nullptr);
        jclass cApplicationInfo = (*env).FindClass("android/content/pm/ApplicationInfo");
        jfieldID fClassName = (*env).GetFieldID(cApplicationInfo, "className",
                                                "Ljava/lang/String;");
        (*env).SetObjectField(oAppInfo0, fClassName, sAppName);
        LOG_D("10");
        jfieldID fAppInfo = (*env).GetFieldID(cAppBindData, "appInfo",
                                              "Landroid/content/pm/ApplicationInfo;");
        jobject oAppInfo1 = (*env).GetObjectField(oMBoundApplication, fAppInfo);
        LOG_D("11");
        assert(oAppInfo1 != nullptr);
        (*env).SetObjectField(oAppInfo1, fClassName, sAppName);
        LOG_D("12");
        jmethodID mMakeApplication = (*env).GetMethodID(
                cLoadedApk, "makeApplication",
                "(ZLandroid/app/Instrumentation;)Landroid/app/Application;");
        jobject oApp = (*env).CallObjectMethod(oLoadedApkInfo, mMakeApplication,
                                               JNI_FALSE, nullptr);
        oApplication = (*env).NewGlobalRef(oApp);
        LOG_D("13");
        (*env).SetObjectField(oCurrentActivityThread, fMInitialApplication, oApplication);
        LOG_D("14");
        // change ContentProvider
        jfieldID fMProviderMap;
        if (android_get_device_api_level() >= __ANDROID_API_K__) {
            fMProviderMap = (*env).GetFieldID(cActivityThread, "mProviderMap",
                                              "Landroid/util/ArrayMap;");
        } else {
            fMProviderMap = (*env).GetFieldID(cActivityThread, "mProviderMap",
                                              "Landroid/util/HashMap;");
        }
        jobject oMProviderMap = (*env).GetObjectField(oCurrentActivityThread, fMProviderMap);
        LOG_D("15");
        assert(oMProviderMap != nullptr);
        jclass cMap;
        if (android_get_device_api_level() >= __ANDROID_API_K__) {
            cMap = (*env).FindClass("android/util/ArrayMap");
        } else {
            cMap = (*env).FindClass("java/util/HashMap");
        }
        jmethodID mValues = (*env).GetMethodID(cMap, "values", "()Ljava/util/Collection;");
        jobject oValues = (*env).CallObjectMethod(oMProviderMap, mValues);
        jclass cCollection = (*env).GetObjectClass(oValues);
        jmethodID mIterator = (*env).GetMethodID(cCollection, "iterator", "()Ljava/util/Iterator;");
        jobject oIterator = (*env).CallObjectMethod(oValues, mIterator);
        jclass cIterator = (*env).GetObjectClass(oIterator);
        jmethodID mHasNext = (*env).GetMethodID(cIterator, "hasNext", "()Z");
        jmethodID mNext = (*env).GetMethodID(cIterator, "next", "()Ljava/lang/Object;");
        jclass cProviderClientRecord = (*env).FindClass(
                "android/app/ActivityThread$ProviderClientRecord");
        jfieldID fMLocalProvider = (*env).GetFieldID(cProviderClientRecord, "mLocalProvider",
                                                     "Landroid/content/ContentProvider;");
        jclass cContentProvider = (*env).FindClass("android/content/ContentProvider");
        jfieldID fMContext = (*env).GetFieldID(cContentProvider, "mContext",
                                               "Landroid/content/Context;");
        jclass cApplication = (*env).GetObjectClass(oApp);
        jmethodID mGetApplicationContext = (*env).GetMethodID(
                cApplication, "getApplicationContext", "()Landroid/content/Context;");
        jobject oContext1 = (*env).CallObjectMethod(oApplication, mGetApplicationContext);
        assert(oContext1 != nullptr);
        LOG_D("16");
        while ((*env).CallBooleanMethod(oIterator, mHasNext)) {
            jobject oProviderClientRecord = (*env).CallObjectMethod(oIterator, mNext);
            jobject oMLocalProvider = (*env).GetObjectField(oProviderClientRecord, fMLocalProvider);
            if(oMLocalProvider!= nullptr){
                (*env).SetObjectField(oMLocalProvider, fMContext, oContext1);
            }
        }
        LOG_D("17");
        (*env).DeleteLocalRef(cActivityThread);
        (*env).DeleteLocalRef(cApplicationInfo);
        (*env).DeleteLocalRef(cAppBindData);
        (*env).DeleteLocalRef(cApplication);
        (*env).DeleteLocalRef(cArrayList);
        (*env).DeleteLocalRef(cCollection);
        (*env).DeleteLocalRef(cContentProvider);
        (*env).DeleteLocalRef(cIterator);
        (*env).DeleteLocalRef(cLoadedApk);
        (*env).DeleteLocalRef(cProviderClientRecord);
        (*env).DeleteLocalRef(cMap);
    }
    LOG_D("99.9999999999");
    jclass c2Application = (*env).GetObjectClass(oApplication);
    jmethodID mOnCreate = (*env).GetMethodID(c2Application, "onCreate", "()V");
    (*env).CallVoidMethod(oApplication, mOnCreate);
    (*env).DeleteLocalRef(c2Application);
    LOG_D("finish, changeTopApplication()");
}

/**
 * get JNIEnv, the version usually is 1.4.
 * init global variable
 * @param vm
 * @param unused
 * @return JNI_VERSION_1_4
 */
jint JNI_OnLoad(JavaVM *vm, void *unused) {
    LOG_D("start, JNI_OnLoad(JavaVM *vm, void* unused)");

    // get JNIEnv
    if ((*vm).GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);

    initConfigFileProxy(&gConfigFileProxy, env);
    buildFileSystem();
    initConfigFile();
    initDexFileHelper(&gDexFileHelper, gConfigFileProxy);

    // change src_app's application
    changeTopApplication();
    return JNI_VERSION_1_4;
}
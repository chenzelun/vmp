#include <jni.h>
#include <string>

#include <android/log.h>
#include <dlfcn.h>

#include "include/AndHook.h"

using namespace std;

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,__FUNCTION__,__VA_ARGS__) // 定义LOGD类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,__FUNCTION__,__VA_ARGS__) // 定义LOGE类型

void test(JNIEnv* env){
    jclass clazz = (*env).FindClass("java/lang/NullPointerException");
    (*env).ThrowNew(clazz, "6666666");
    (*env).NewStringUTF("999");
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

extern "C" JNIEXPORT jstring JNICALL
Java_com_chend_testapp_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

//    void* handle  = dlopen("/data/data/com.chend.testapp/lib/libhello.so", RTLD_NOW);
//    assert(handle!= nullptr);
//    void* symbol = dlsym(handle, "_Z5helloP7_JNIEnv");
//    assert(symbol!= nullptr);
//
//    const char* (*hello)(JNIEnv* ) = nullptr;
//    hello = reinterpret_cast<const char *(*)(JNIEnv*)>(symbol);

//    test(env);
    std::string haha = "Hello from C++";
//    haha+=hello(env);
    return env->NewStringUTF(haha.c_str());
}


JNIEnv* env = nullptr;
jobject clazzloader = nullptr;
jmethodID sysFindClass = nullptr;
jobject myFindClass(jstring name){
    const char* nameC = (*env).GetStringUTFChars(name, nullptr);
    LOGD("name: ", nameC);
    return (env)->CallObjectMethod(clazzloader, sysFindClass, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_chend_testapp_MainActivity_hookFindClass(JNIEnv *tmp, jobject thiz, jobject class_loader) {
    clazzloader = class_loader;
    env = tmp;
    HookJava(env, "dalvik/system/BaseDexClassLoader", "findClass", "(Ljava/lang/String;)Ljava/lang/Class;",
             reinterpret_cast<const void *>(myFindClass), &sysFindClass);
}extern "C"
JNIEXPORT void JNICALL
Java_com_chend_testapp_MainActivity_onCreate(JNIEnv *env, jobject thiz,
                                             jobject saved_instance_state) {
    // TODO: implement onCreate()
    jclass clazz  = (*env).GetObjectClass(thiz);
    jclass superClazz = (*env).GetSuperclass(clazz);
    jmethodID mOnCreate = (*env).GetMethodID(clazz, "onCreate", "(Landroid/os/Bundle;)V");
    (*env).CallNonvirtualVoidMethod(thiz, superClazz, mOnCreate, saved_instance_state);

    jmethodID mSetContentView = (*env).GetMethodID(clazz, "setContentView", "(I)V");
    (*env).CallVoidMethod(thiz, mSetContentView, -1300009);
}
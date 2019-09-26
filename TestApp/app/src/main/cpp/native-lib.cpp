#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_D(...) __android_log_print(ANDROID_LOG_DEBUG,__FUNCTION__,__VA_ARGS__) // 定义LOGD类型
#define LOG_E(...) __android_log_print(ANDROID_LOG_ERROR,__FUNCTION__,__VA_ARGS__) // 定义LOGE类型

using  namespace std;

extern "C" JNIEXPORT jstring JNICALL
Java_com_chend_testapp_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_chend_testapp_MainActivity_size(JNIEnv *env, jobject thiz, jobjectArray a) {
    // TODO: implement size()
    return (*env).GetArrayLength(a);
}

            
            //
            // Created by ChenDalunlun on 2019-09-24.
            //
            
            #ifndef SHELLAPPLICATION_JNIFUNC_H
            #define SHELLAPPLICATION_JNIFUNC_H
            
            #include <jni.h>
                    
            using namespace std;
        
        
                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_chend_testapp_MainActivity_000241_onClick__Landroid_view_View_2(
                        JNIEnv *env, jobject instance, jobject param0)
            ;

                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_chend_testapp_Main2Activity_onCreate__Landroid_os_Bundle_2(
                        JNIEnv *env, jobject instance, jobject param0)
            ;

                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_chend_testapp_MainActivity_onCreate__Landroid_os_Bundle_2(
                        JNIEnv *env, jobject instance, jobject param0)
            ;
    #endif //SHELLAPP_JNIFUNC_H
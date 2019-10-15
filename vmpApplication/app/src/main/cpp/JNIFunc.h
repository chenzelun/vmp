
            
            //
            // Created by ChenDalunlun on 2019-10-15.
            //
            
            #ifndef SHELLAPPLICATION_JNIFUNC_H
            #define SHELLAPPLICATION_JNIFUNC_H
            
            #include <jni.h>
                    
            using namespace std;
        
        
                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_000241_onClick__Landroid_view_View_2(
                        JNIEnv *env, jobject instance, jobject param0)
            ;

                
                extern "C"
                JNIEXPORT jobject JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_AES128Encrypt___3B_3B_3B(
                        JNIEnv *env, jobject instance, jobject param0, jobject param1, jobject param2)
            ;

                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_createFile__Ljava_io_File_2J(
                        JNIEnv *env, jobject instance, jobject param0, jlong param1)
            ;

                
                extern "C"
                JNIEXPORT jobject JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_matrixMultiple___3_3I_3_3I(
                        JNIEnv *env, jobject instance, jobject param0, jobject param1)
            ;

                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_onCreate__Landroid_os_Bundle_2(
                        JNIEnv *env, jobject instance, jobject param0)
            ;
    #endif //SHELLAPP_JNIFUNC_H

                 
            //
            // Created by ChenDalunlun on 2019-10-21.
            //
            
            #include "JNIFunc.h"
            #include "VMInterpreter.h"
            #include "Util.h"
            #include "common/Dalvik.h"
                    
        
                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_000241_onClick__Landroid_view_View_2(
                        JNIEnv *env, jobject instance, jobject param0)
            {
                
                LOG_D("jni function start: %s", __func__);
                jclass clazz_type = (*env).GetObjectClass(instance);
                jmethodID m_jni_method = (*env).GetMethodID(clazz_type, "onClick", "(Landroid/view/View;)V");
                (*env).DeleteLocalRef(clazz_type);
                // init method
                const VmMethod *method = initVmMethod(m_jni_method);
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue, param0);
                deleteVmMethod(method);
                LOG_D("jni function finish");
            }
                
                extern "C"
                JNIEXPORT jobject JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_AES128Encrypt___3B_3B_3B(
                        JNIEnv *env, jobject instance, jobject param0, jobject param1, jobject param2)
            {
                
                LOG_D("jni function start: %s", __func__);
                jclass clazz_type = (*env).GetObjectClass(instance);
                jmethodID m_jni_method = (*env).GetMethodID(clazz_type, "AES128Encrypt", "([B[B[B)[B");
                (*env).DeleteLocalRef(clazz_type);
                // init method
                const VmMethod *method = initVmMethod(m_jni_method);
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue, param0, param1, param2);
                deleteVmMethod(method);
                LOG_D("jni function finish");
            
                return retValue.l;
                }
                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_createFile__Ljava_io_File_2J(
                        JNIEnv *env, jobject instance, jobject param0, jlong param1)
            {
                
                LOG_D("jni function start: %s", __func__);
                jclass clazz_type = (*env).GetObjectClass(instance);
                jmethodID m_jni_method = (*env).GetMethodID(clazz_type, "createFile", "(Ljava/io/File;J)V");
                (*env).DeleteLocalRef(clazz_type);
                // init method
                const VmMethod *method = initVmMethod(m_jni_method);
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue, param0, param1);
                deleteVmMethod(method);
                LOG_D("jni function finish");
            }
                
                extern "C"
                JNIEXPORT jobject JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_matrixMultiple___3_3I_3_3I(
                        JNIEnv *env, jobject instance, jobject param0, jobject param1)
            {
                
                LOG_D("jni function start: %s", __func__);
                jclass clazz_type = (*env).GetObjectClass(instance);
                jmethodID m_jni_method = (*env).GetMethodID(clazz_type, "matrixMultiple", "([[I[[I)[[I");
                (*env).DeleteLocalRef(clazz_type);
                // init method
                const VmMethod *method = initVmMethod(m_jni_method);
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue, param0, param1);
                deleteVmMethod(method);
                LOG_D("jni function finish");
            
                return retValue.l;
                }
                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_example_vmp_1performancetest_MainActivity_onCreate__Landroid_os_Bundle_2(
                        JNIEnv *env, jobject instance, jobject param0)
            {
                
                LOG_D("jni function start: %s", __func__);
                jclass clazz_type = (*env).GetObjectClass(instance);
                jmethodID m_jni_method = (*env).GetMethodID(clazz_type, "onCreate", "(Landroid/os/Bundle;)V");
                (*env).DeleteLocalRef(clazz_type);
                // init method
                const VmMethod *method = initVmMethod(m_jni_method);
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue, param0);
                deleteVmMethod(method);
                LOG_D("jni function finish");
            }
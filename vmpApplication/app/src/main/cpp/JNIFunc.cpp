
                
            #include "JNIFunc.h"
            #include "VMInterpreter.h"
            #include "Util.h"
            #include "common/Dalvik.h"
                    
        
                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_chend_testapp_MainActivity_000241_onClick__Landroid_view_View_2(
                        JNIEnv *env, jobject instance, jobject param0)
            {
                
                LOG_D("jni function start: %s", __func__);
                jclass clazz_type = (*env).GetObjectClass(instance);
                jmethodID m_onClick = (*env).GetMethodID(clazz_type, "onClick", "(Landroid/view/View;)V");
                // init method
                const VmMethod *method = initVmMethod(m_onClick);
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue, param0);
                deleteVmMethod(method);
                LOG_D("jni function finish");
            }
                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_chend_testapp_Main2Activity_onCreate__Landroid_os_Bundle_2(
                        JNIEnv *env, jobject instance, jobject param0)
            {
                
                LOG_D("jni function start: %s", __func__);
                jclass clazz_type = (*env).GetObjectClass(instance);
                jmethodID m_onCreate = (*env).GetMethodID(clazz_type, "onCreate", "(Landroid/os/Bundle;)V");
                // init method
                const VmMethod *method = initVmMethod(m_onCreate);
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue, param0);
                deleteVmMethod(method);
                LOG_D("jni function finish");
            }
                
                extern "C"
                JNIEXPORT void JNICALL
                Java_com_chend_testapp_MainActivity_onCreate__Landroid_os_Bundle_2(
                        JNIEnv *env, jobject instance, jobject param0)
            {
                
                LOG_D("jni function start: %s", __func__);
                jclass clazz_type = (*env).GetObjectClass(instance);
                jmethodID m_onCreate = (*env).GetMethodID(clazz_type, "onCreate", "(Landroid/os/Bundle;)V");
                // init method
                const VmMethod *method = initVmMethod(m_onCreate);
                jvalue retValue;
                dvmCallMethod(env, instance, method, &retValue, param0);
                deleteVmMethod(method);
                LOG_D("jni function finish");
            }
#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/asset_manager.h>

//extern "C" {

//}

//jint JNI_OnLoad(JavaVM* jvm, void* reserved) {
//    JNIEnv* env = NULL;
//    if((*jvm)->GetEnv
//}

extern "C" JNIEXPORT jstring JNICALL
Java_com_sty_ne_jnilogin_md5_encrypt_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

/**
 * Class:  com_sty_ne_jnilogin_md5_encrypt_UserInfo
 * Method: nativeEncrypt
 * Signature: ()V
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_sty_ne_jnilogin_md5_encrypt_UserInfo_nativeEncrypt(JNIEnv *env, jobject instance) {
    jclass cls = env->GetObjectClass(instance);
    jfieldID pwdField = env->GetFieldID(cls, "userPwd", "Ljava/lang/String;");
    jstring strPwd = static_cast<jstring>(env->GetObjectField(instance, pwdField));

    jclass clsMd5 = env->FindClass("com/sty/ne/jnilogin/md5/encrypt/MD5Helper");
    jmethodID md5Method = env->GetStaticMethodID(clsMd5, "getMD5", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring strNewPwd = static_cast<jstring>(env->CallStaticObjectMethod(clsMd5, md5Method,
                                                                         strPwd));
    env->SetObjectField(instance, pwdField, strNewPwd);
}
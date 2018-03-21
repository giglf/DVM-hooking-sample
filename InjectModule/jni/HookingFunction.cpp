#include <stdio.h>
#include <stdlib.h>
#include <jni.h>

#include "HookingFunction.h"


extern "C" JNIEXPORT jboolean JNICALL hacked_hasPrimaryClip(JNIEnv *env, jclass clazz){
	return JNI_TRUE;
}

extern "C" JNIEXPORT jobject JNICALL hacked_getPrimaryClip(JNIEnv * env, jclass clazz){
	
	jclass ClipData = env->FindClass("android/content/ClipData");
	jmethodID newPlainTextID = env->GetStaticMethodID(ClipData, "newPlainText", "(Ljava/lang/CharSequence;Ljava/lang/CharSequence;)Landroid/content/ClipData;");
	jstring param = env->NewStringUTF("clipboard_hacked_by_giglf!");

	return env->CallStaticObjectMethod(ClipData, newPlainTextID, NULL, param);

}

extern "C" JNIEXPORT jstring JNICALL test(JNIEnv *env, jclass clazz)  
{  
    //__android_log_print(ANDROID_LOG_VERBOSE, "tag", "call <native_printf> in java");
    return env->NewStringUTF("haha");
}

HookInfo hookInfos[] = {
	{"android/net/wifi/WifiInfo","getMacAddress","()Ljava/lang/String;",(void*)test},
	// {"android.content.IClipboard", "hasPrimaryClip", "()Z", (void*)hacked_hasPrimaryClip},
	// {"android.content.IClipboard", "getPrimaryClip", "()Landroid/content/ClipData;", (void*)hacked_getPrimaryClip}
};

int getpHookInfo(HookInfo** pInfo){
	*pInfo = hookInfos;
	return sizeof(hookInfos) / sizeof(hookInfos[0]);
}

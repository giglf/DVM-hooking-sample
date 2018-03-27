#include <android_runtime/AndroidRuntime.h>
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "JavaHook/JavaMethodHook.h"
#include "ElfHook/elfutils.h"
#include "ElfHook/elfhook.h"
#include "common.h"


static inline void get_cstr_from_jstring(JNIEnv* env, jstring jstr, char **out) {
	jboolean iscopy = JNI_TRUE;
	const char *cstr = env->GetStringUTFChars(jstr, &iscopy);
	*out = strdup(cstr);
	env->ReleaseStringUTFChars(jstr, cstr);
}

static jstring test(JNIEnv *env, jobject clazz)  
{  
    return env->NewStringUTF("ahahahahah have been hooked!");;
}

HookInfo hookInfos[] = {
		{"android/net/wifi/WifiInfo","getMacAddress","()Ljava/lang/String;",(void*)test, false}
		//{"com/example/testar/MainActivity","test","()Ljava/lang/String;",(void*)test},
		//{"android/app/ApplicationLoaders","getText","()Ljava/lang/CharSequence;",(void*)test}
};

// static JavaVM *javavm;
// HookInfo hookinfo;

extern "C" void HookEntry(){
	
	JNIEnv *env = android::AndroidRuntime::getJNIEnv();

	
	// hookinfo.classDesc = "android/net/wifi/WifiInfo";
	// hookinfo.methodName = "getMacAddress";
	// hookinfo.methodSig = "()Ljava/lang/String;";
	// hookinfo.isStaticMethod = false;
	// hookinfo.originalMethod = NULL;
	// hookinfo.returnType = NULL;
	// hookinfo.paramTypes = NULL;

	java_method_hook(env, &hookInfos[0]);

}


#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#define ANDROID_SMP 0
#include <Dalvik.h>
#include <alloc/Alloc.h>

#include "HookingFunction.h"

static bool g_bAttatedT;
static JavaVM *g_JavaVM;

void init()
{
	g_bAttatedT = false;	
	g_JavaVM = android::AndroidRuntime::getJavaVM();
}

static JNIEnv *GetEnv()
{
	int status;
	JNIEnv *envnow = NULL;
	status = g_JavaVM->GetEnv((void **)&envnow, JNI_VERSION_1_4);
	if(status < 0)
	{
		status = g_JavaVM->AttachCurrentThread(&envnow, NULL);
		if(status < 0)
		{
			return NULL;
		}
		g_bAttatedT = true;
	}
	return envnow;
}

static void DetachCurrent()
{
	if(g_bAttatedT)
	{
		g_JavaVM->DetachCurrentThread();
	}
}

static int computeJniArgInfo(const DexProto* proto)
{
    const char* sig = dexProtoGetShorty(proto);
    int returnType, jniArgInfo;
    u4 hints;

    /* The first shorty character is the return type. */
    switch (*(sig++)) {
    case 'V':
        returnType = DALVIK_JNI_RETURN_VOID;
        break;
    case 'F':
        returnType = DALVIK_JNI_RETURN_FLOAT;
        break;
    case 'D':
        returnType = DALVIK_JNI_RETURN_DOUBLE;
        break;
    case 'J':
        returnType = DALVIK_JNI_RETURN_S8;
        break;
    case 'Z':
    case 'B':
        returnType = DALVIK_JNI_RETURN_S1;
        break;
    case 'C':
        returnType = DALVIK_JNI_RETURN_U2;
        break;
    case 'S':
        returnType = DALVIK_JNI_RETURN_S2;
        break;
    default:
        returnType = DALVIK_JNI_RETURN_S4;
        break;
    }

    jniArgInfo = returnType << DALVIK_JNI_RETURN_SHIFT;

    hints = dvmPlatformInvokeHints(proto);

    if (hints & DALVIK_JNI_NO_ARG_INFO) {
        jniArgInfo |= DALVIK_JNI_NO_ARG_INFO;
    } else {
        assert((hints & DALVIK_JNI_RETURN_MASK) == 0);
        jniArgInfo |= hints;
    }

    return jniArgInfo;
}

int ClearException(JNIEnv *jenv){
	jthrowable exception = jenv->ExceptionOccurred();
	if (exception != NULL) {
		jenv->ExceptionDescribe();
		jenv->ExceptionClear();
		return true;
	}
	return false;
}

/* TODO: need to fix*/
bool isArt(){
	return false;
}

static jclass findAppClass(JNIEnv *jenv,const char *apn){
	//获取Loaders
	jclass clazzApplicationLoaders = jenv->FindClass("android/app/ApplicationLoaders");
	jthrowable exception = jenv->ExceptionOccurred();
	if (ClearException(jenv)) {
		ALOGW("No class : %s", "android/app/ApplicationLoaders");
		return NULL;
	}
	jfieldID fieldApplicationLoaders = jenv->GetStaticFieldID(clazzApplicationLoaders,"gApplicationLoaders","Landroid/app/ApplicationLoaders;");
	if (ClearException(jenv)) {
		ALOGW("No Static Field :%s","gApplicationLoaders");
		return NULL;
	}
	jobject objApplicationLoaders = jenv->GetStaticObjectField(clazzApplicationLoaders,fieldApplicationLoaders);
	if (ClearException(jenv)) {
		ALOGW("GetStaticObjectField is failed [%s","gApplicationLoaders");
		return NULL;
	}
	jfieldID fieldLoaders = jenv->GetFieldID(clazzApplicationLoaders,"mLoaders","Ljava/util/Map;");
	if (ClearException(jenv)) {
		ALOGW("No Field :%s","mLoaders");
		return NULL;
	}
	jobject objLoaders = jenv->GetObjectField(objApplicationLoaders,fieldLoaders);
	if (ClearException(jenv)) {
		ALOGW("No object :%s","mLoaders");
		return NULL;
	}
	//Map mLoaders = gApplicationLoaders.mLoaders;

	//提取map中的values
	jclass clazzHashMap = jenv->GetObjectClass(objLoaders);
	jmethodID methodValues = jenv->GetMethodID(clazzHashMap,"values","()Ljava/util/Collection;");
	jobject values = jenv->CallObjectMethod(objLoaders,methodValues);
	//Collection values = mLoaders.values();

	jclass clazzValues = jenv->GetObjectClass(values);
	jmethodID methodToArray = jenv->GetMethodID(clazzValues,"toArray","()[Ljava/lang/Object;");
	if (ClearException(jenv)) {
		ALOGW("No Method:%s","toArray");
		return NULL;
	}
	//Object array = values.toArray();

	jobjectArray classLoaders = (jobjectArray)jenv->CallObjectMethod(values,methodToArray);
	if (ClearException(jenv)) {
		ALOGW("CallObjectMethod failed :%s","toArray");
		return NULL;
	}

	int size = jenv->GetArrayLength(classLoaders);

	//load class from Classloader array. Looping until the class from param success load.
	for(int i = 0 ; i < size ; i ++){
		jobject classLoader = jenv->GetObjectArrayElement(classLoaders,i);
		jclass clazzCL = jenv->GetObjectClass(classLoader);
		jmethodID loadClass = jenv->GetMethodID(clazzCL,"loadClass","(Ljava/lang/String;)Ljava/lang/Class;");
		jstring param = jenv->NewStringUTF(apn);
		jclass tClazz = (jclass)jenv->CallObjectMethod(classLoader,loadClass,param);
		if (ClearException(jenv)) {
			ALOGW("No");
			continue;
		}
		return tClazz;
	}
	ALOGW("No");
	return NULL;
}



bool HookDalvikMethod(jmethodID jmethod){
	Method *method = (Method*)jmethod;
	//关键!!将目标方法修改为native方法
	SET_METHOD_FLAG(method, ACC_NATIVE);

	int argsSize = dvmComputeMethodArgsSize(method);
	ALOGW("Jmethod argsSize = %d ", argsSize);
    if (!dvmIsStaticMethod(method))
        argsSize++;

    method->registersSize = method->insSize = argsSize;

    if (dvmIsNativeMethod(method)) {
        method->nativeFunc = dvmResolveNativeMethod;
        method->jniArgInfo = computeJniArgInfo(&method->prototype);
    }
}

bool ClassMethodHook(HookInfo info){

	JNIEnv *jenv = GetEnv();

	// Load class from JNI environment first. If not found, continue searching in Application environment.
	jclass clazzTarget = jenv->FindClass(info.tClazz);
	if (ClearException(jenv)) {
		ALOGW("ClassMethodHook Can't find class:%s in bootclassloader",info.tClazz);

	    clazzTarget = findAppClass(jenv,info.tClazz);
	    if(clazzTarget == NULL){
	    	ALOGW("%s","Error in findAppClass");
	    	return false;
	    }
	}

	jmethodID method = jenv->GetMethodID(clazzTarget,info.tMethod,info.tMeihodSig);
	if(method==NULL){
		ALOGW("ClassMethodHook Can't find method:%s",info.tMethod);
		return false;
	}

	if(isArt()){
		// HookArtMethod(jenv,method); TODO: not implement
	}else{
		HookDalvikMethod(method);
	}

    JNINativeMethod gMethod[] = {
        {info.tMethod, info.tMeihodSig, info.handleFunc},
    };

    //func为NULL时不自行绑定,后面扩展吧
    if(info.handleFunc != NULL){
		//关键!!将目标方法关联到自定义的native方法
		if (jenv->RegisterNatives(clazzTarget, gMethod, 1) < 0) {
			ALOGW("RegisterNatives err");
			return false;
		}
    }

	DetachCurrent();
	return true;
}



int Hook(){

	ALOGW("Inject function Hook() call");

	init();

	HookInfo *hookInfo;
	getpHookInfo(&hookInfo);

	ALOGW("Target Class:%s", hookInfo[0].tClazz);
	ALOGW("Target Method:%s", hookInfo[0].tMethod);

	ClassMethodHook(hookInfo[0]);
	// ClassMethodHook(hookInfo[1]);
}

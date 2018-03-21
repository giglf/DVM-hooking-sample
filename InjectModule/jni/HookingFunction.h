

#ifndef HOOKING_FUNCTION_H
#define HOOKING_FUNCTION_H
 
#include <android/log.h>

#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN,"Hacked_by_giglf", __VA_ARGS__); 

typedef struct{
	const char *tClazz;
	const char *tMethod;
	const char *tMeihodSig;
	void *handleFunc;
} HookInfo;

int getpHookInfo(HookInfo** pInfo);

#endif
/************************************************************
  FileName: ptraceInject.h
  Description:       ptrace注入      
***********************************************************/

#include <stdio.h>    
#include <stdlib.h>       
#include <unistd.h>    
#include <android/log.h>

#define  MAX_PATH 0x100

#define LOG_TAG "Hacked_by_giglf"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__))

int inject_remote_process(pid_t pid, char *LibPath, char *FunctionName, long *FuncParameter, long NumParameter);   // 通过ptrace远程调用dlopen/dlsym方式注入模块到远程进程
int inject_remote_process_shellcode(pid_t pid, char *LibPath, char *FunctionName, long *FuncParameter, long NumParameter); // 通过shellcode方式注入模块到远程进程
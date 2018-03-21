LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := inject
LOCAL_SRC_FILES := ptraceInject.c main.c shellcode.s

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

include $(BUILD_EXECUTABLE)
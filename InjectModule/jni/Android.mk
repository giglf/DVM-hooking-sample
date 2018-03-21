LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := injectModule
LOCAL_SRC_FILES := HookingEntry.cpp MethodHooker.cpp HookingFunction.cpp

LOCAL_ARM_MODE := arm

LOCAL_CFLAGS    := -I./jni/include/ -I./jni/dalvik/vm/ -I./jni/dalvik -DHAVE_LITTLE_ENDIAN

LOCAL_LDLIBS	:=	-L./jni/lib/  -L$(SYSROOT)/usr/lib -llog -ldvm -landroid_runtime  -lart -lutils -lcutils
# LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog -lz

LOCAL_STATIC_LIBRARIES := cpufeatures

include $(BUILD_SHARED_LIBRARY)


$(call import-module,android/cpufeatures)


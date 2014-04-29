LOCAL_PATH := $(call my-dir)

# libvge.so
include $(CLEAR_VARS)
LOCAL_MODULE    := vgs2
LOCAL_SRC_FILES :=../../src/vgs2a.c ../../src/vgs2api.c ../../src/game.c
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) ../../src
LOCAL_LDLIBS := -ljnigraphics -lOpenSLES -llog
LOCAL_LDFLAGS := 
LOCAL_CFLAGS := -I ../../src
CAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)


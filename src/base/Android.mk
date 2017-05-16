LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -D__DEBUG__ -g
#APP_PLATFORM := android-23

LOCAL_C_INCLUDES	:=	\
	$(LOCAL_PATH)/../base 			\
	$(LOCAL_PATH)/../base/android 	\
	$(LOCAL_PATH)/../filter 		

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 


#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_MODULE := gpu

include $(BUILD_STATIC_LIBRARY)

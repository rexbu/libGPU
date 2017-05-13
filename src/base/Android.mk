LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -D__DEBUG__ -g
#APP_PLATFORM := android-23

LOCAL_C_INCLUDES	:=	\
	$(LOCAL_PATH)/../../mc	\
	$(LOCAL_PATH)/../../mc/basic	\
	$(LOCAL_PATH)/../../mc/bs 		\
	$(LOCAL_PATH)/../../mc/me		\
	$(LOCAL_PATH)/../../third/sszip	\
	$(LOCAL_PATH)/../../third/cjson	\
	$(LOCAL_PATH)/../../third/oepnssl 			\
	$(LOCAL_PATH)/../../third/openssl/include 	\
	$(LOCAL_PATH)/../../third/curl				\
	$(LOCAL_PATH)/../../third/curl/include 		\
	$(LOCAL_PATH)/../../third/curl/include/curl \
	$(LOCAL_PATH)/../base 			\
	$(LOCAL_PATH)/../base/android 	\
	$(LOCAL_PATH)/../filter 		\
	$(LOCAL_PATH)/../core

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_SRC_FILES += $(SRCS)
# LOCAL_SRC_FILES += 		\
# 	GPUBasic.cpp		\
# 	GPUContext.cpp		\
# 	GPUCropFilter.cpp	\
# 	GPUDualport.cpp		\
# 	GPUEglContext.cpp	\
# 	GPUFilter.cpp		\
# 	GPUFilterZ.cpp		\
# 	GPUFilterBlend.cpp	\
# 	GPUFilterBox.cpp	\
# 	GPUFrameBuffer.cpp	\
# 	GPUGroupFilter.cpp	\
# 	GPUInput.cpp		\
# 	GPUMutableInputFilter.cpp\
# 	GPUOutput.cpp		\
# 	GPUPixelBuffer.cpp	\
# 	GPUProgram.cpp		\
# 	GPURawInput.cpp		\
# 	GPURawOutput.cpp	\
# 	GPUTwoInputFilter.cpp	\
# 	GPUTwoPassFilter.cpp	\
# 	GPUView.cpp			\
# 	GPUVertexBuffer.cpp	\
# 	GPUYUV420Filter.cpp	\
# 	GPUYUVFilter.cpp	\
# 	GPUJointFilter.cpp	\
# 	GPUZoomFilter.cpp	


#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_MODULE := gpu

include $(BUILD_STATIC_LIBRARY)

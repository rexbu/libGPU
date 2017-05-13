/**
 * file :	GPUPicture.cpp
 * author :	Rex
 * create :	2016-12-13 15:53
 * func : 
 * history:
 */

#include <android/bitmap.h>
#include "GPUPicture.h"

extern JavaVM*  g_jvm;
GPUPicture::GPUPicture(uint8_t * data, uint32_t size){
	JNIEnv*     	env;
    g_jvm->AttachCurrentThread(&env, NULL);
    
	jbyteArray  jbarray = env->NewByteArray(size);
	env->SetByteArrayRegion(jbarray, 0, size, (jbyte*)data); 

	jclass jc= env->FindClass("com/rex/utils/FileUtil");
	jmethodID method = env->GetStaticMethodID(jc, "readPNG", "([B)Landroid/graphics/Bitmap;");
	jobject bitmap = (jstring)env->CallStaticObjectMethod(jc, method, jbarray);
	env->DeleteLocalRef(jbarray);
	m_exist = true;
	if (bitmap==NULL)
	{
		err_log("Parse bytes[%d] to Bitmap Error. ", size);
		m_exist = false;
		return;
	}

	AndroidBitmapInfo info;
	GLvoid* pixels;

	AndroidBitmap_getInfo(env, bitmap, &info);
	if(info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		err_log("Bitmap format[%d] is not RGBA_8888!", info.format);
		return;
	}

	AndroidBitmap_lockPixels(env, bitmap, &pixels);
	setPixel((uint8_t*)pixels, info.width, info.height);
	AndroidBitmap_unlockPixels(env, bitmap);
	// 主线程不能销毁
	// g_jvm->DetachCurrentThread();
}

GPUPicture::GPUPicture(const char* path){
    JNIEnv*     	env;
    g_jvm->AttachCurrentThread(&env, NULL);
    
	jstring jpath = env->NewStringUTF(path);
	jclass jc= env->FindClass("com/rex/utils/FileUtil");
	jmethodID method = env->GetStaticMethodID(jc, "readPNG", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
	jobject bitmap = (jstring)env->CallStaticObjectMethod(jc, method, jpath);
	m_exist = true;
	if (bitmap==NULL)
	{
		m_exist = false;
		return;
	}

	AndroidBitmapInfo info;
	GLvoid* pixels;

	AndroidBitmap_getInfo(env, bitmap, &info);
	if(info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		err_log("Bitmap format[%d] is not RGBA_8888!", info.format);
		return;
	}
	
	AndroidBitmap_lockPixels(env, bitmap, &pixels);
	setPixel((uint8_t*)pixels, info.width, info.height);
	AndroidBitmap_unlockPixels(env, bitmap);
	// g_jvm->DetachCurrentThread();
}
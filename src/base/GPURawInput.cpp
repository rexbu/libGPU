/**
 * file :	GPURawInput.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 21:10
 * func : 
 * history:
 */

#include "GPURawInput.h"
#include "GPUPixelBuffer.h"
#include "GPUYUVFilter.h"

GPURawInput::GPURawInput(gpu_pixel_format_t format){
    m_input = NULL;
    setFormat(format);
}
GPURawInput::GPURawInput(int width, int height, gpu_pixel_format_t format){
    m_width = width;
    m_height = height;

    m_input = NULL;
    setFormat(format);
}

void GPURawInput::setFormat(gpu_pixel_format_t format){
    if (m_input!=NULL){
        delete m_input;
    }

    switch (format){
        case GPU_RGBA:
            m_input = new GPUFilter();
            break;
        case GPU_NV21:
            m_input = new GPUNV21ToRGBFilter();
            break;
        case GPU_NV12:
            m_input = new GPUNV12ToRGBFilter();
            break;
        case GPU_I420:
            m_input = new GPUI420ToRGBFilter();
            break;
    }
    m_in_format = format;
}

void GPURawInput::uploadBytes(GLubyte* bytes){
    uploadBytes(bytes, m_width, m_height, m_in_format);
}
void GPURawInput::uploadBytes(GLubyte* bytes, int width, int height, gpu_pixel_format_t in_type){
    // m_width和m_height是输出尺寸，和此处width、height可能为旋转关系，注意区别
    if(in_type != m_in_format){
        setFormat(in_type);
    }

    if (in_type==GPU_RGBA) {

        GPUFrameBuffer* buffer = GPUBufferCache::shareInstance()->getFrameBuffer(width, height);
        glBindTexture(GL_TEXTURE_2D, buffer->m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        m_input->setInputFrameBuffer(buffer);
    }
    else if(in_type==GPU_NV21 || in_type==GPU_NV12){
        GPUContext* context = GPUContext::shareInstance();
        context->glContextLock();   // 加锁，防止此时设置参数
        context->makeCurrent();

        GPUFrameBuffer* y_buffer = GPUBufferCache::shareInstance()->getFrameBuffer(width, height);
        glBindTexture(GL_TEXTURE_2D, y_buffer->m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, bytes);
        
        GPUFrameBuffer* vu_buffer = GPUBufferCache::shareInstance()->getFrameBuffer(width/2, height/2);
        glBindTexture(GL_TEXTURE_2D, vu_buffer->m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width/2, height/2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, bytes+width*height);

        context->glContextUnlock();

        m_input->setInputFrameBuffer(y_buffer, 0);
        m_input->setInputFrameBuffer(vu_buffer, 1);

    }
    else if(in_type == GPU_I420){
        m_input = new GPUI420ToRGBFilter();
        GPUContext* context = GPUContext::shareInstance();
        context->glContextLock();   // 加锁，防止此时设置参数
        context->makeCurrent();

        GPUFrameBuffer* y_buffer = GPUBufferCache::shareInstance()->getFrameBuffer(width, height);
        glBindTexture(GL_TEXTURE_2D, y_buffer->m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, bytes);
        
        GPUFrameBuffer* u_buffer = GPUBufferCache::shareInstance()->getFrameBuffer(width/2, height/2);
        glBindTexture(GL_TEXTURE_2D, u_buffer->m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width/2, height/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, bytes+width*height);

        GPUFrameBuffer* v_buffer = GPUBufferCache::shareInstance()->getFrameBuffer(width/2, height/2);
        glBindTexture(GL_TEXTURE_2D, v_buffer->m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width/2, height/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, bytes+width*height*5/4);

        context->glContextUnlock();

        m_input->setInputFrameBuffer(y_buffer, 0);
        m_input->setInputFrameBuffer(u_buffer, 1);
        m_input->setInputFrameBuffer(v_buffer, 2);
    }
    m_input->render();
    m_outbuffer = m_input->m_outbuffer;
    informTargets();
}

GPURawInput::~GPURawInput(){
    if (m_input!=NULL) {
        delete m_input;
    }
}

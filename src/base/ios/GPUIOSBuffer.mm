/**
 * file :	GPUIOSBuffer.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-07-08 16:08
 * func : 
 * history:
 */

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>
#include "GPUIOSBuffer.h"
#include "GPUContext.h"

GPUIOSFrameBuffer::GPUIOSFrameBuffer(gpu_size_t size, bool only_texture):
m_render_target(nil){
    init(size.width, size.height, GPUFrameBuffer::defaultTextureOption(), only_texture);
}

GPUIOSFrameBuffer::GPUIOSFrameBuffer(int width, int height, bool only_texture):
m_render_target(nil){
    init(width, height, GPUFrameBuffer::defaultTextureOption(), only_texture);
}

GPUIOSFrameBuffer::GPUIOSFrameBuffer(int width, int height, GPUTextureOption_t option, bool only_texture):
m_render_target(nil){
    init(width, height, option, only_texture);
}

GPUIOSFrameBuffer::GPUIOSFrameBuffer(int width, int height, GLuint texture):
m_render_target(nil),
GPUFrameBuffer(width, height, texture){}

void GPUIOSFrameBuffer::generateFrameBuffer(){
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glGenRenderbuffers(1, &m_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    
    CVOpenGLESTextureCacheRef coreVideoTextureCache =  (CVOpenGLESTextureCacheRef)GPUContext::shareInstance()->coreVideoTextureCache();
    // Code originally sourced from http://allmybrain.com/2011/12/08/rendering-to-a-texture-with-ios-5-texture-cache-api/
    
    CFDictionaryRef empty; // empty value for attr value.
    CFMutableDictionaryRef attrs;
    empty = CFDictionaryCreate(kCFAllocatorDefault, NULL, NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks); // our empty IOSurface properties dictionary
    attrs = CFDictionaryCreateMutable(kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(attrs, kCVPixelBufferIOSurfacePropertiesKey, empty);
    
    CVReturn err = CVPixelBufferCreate(kCFAllocatorDefault, (int)m_width, (int)m_height, kCVPixelFormatType_32BGRA, attrs, &m_render_target);
    if (err)
    {
        err_log("FBO size: %d, %d", m_width, m_height);
        err_log("Error at CVPixelBufferCreate %d", err);
        exit(-1);
    }
    
    err = CVOpenGLESTextureCacheCreateTextureFromImage (kCFAllocatorDefault, coreVideoTextureCache, m_render_target,
                                                        NULL, // texture attributes
                                                        GL_TEXTURE_2D,
                                                        m_texture_option.internal_format, // opengl format
                                                        (int)m_width,
                                                        (int)m_height,
                                                        m_texture_option.format, // native iOS format
                                                        m_texture_option.type,
                                                        0,
                                                        &m_render_texture);
    if (err)
    {
        err_log("Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        exit(-1);
    }
    
    CFRelease(attrs);
    CFRelease(empty);
    
    glBindTexture(CVOpenGLESTextureGetTarget(m_render_texture), CVOpenGLESTextureGetName(m_render_texture));
    m_texture = CVOpenGLESTextureGetName(m_render_texture);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_texture_option.min_filter);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_texture_option.mag_filter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_texture_option.wrap_s);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_texture_option.wrap_t);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CVOpenGLESTextureGetName(m_render_texture), 0);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
            
        case GL_FRAMEBUFFER_UNSUPPORTED:
            err_log("fbo unsupported");
            break;
            
        default:
            err_log("Framebuffer Error: %x", status);
            break;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

GPUIOSFrameBuffer::~GPUIOSFrameBuffer(){
    CFRelease(m_render_target);
    m_render_target = nil;
    CFRelease(m_render_texture);
    m_render_texture = nil;
}

/////////////////// GPUIOSBuffer /////////////////////
GPUBufferCache* GPUIOSBufferCache::shareInstance(){
    if (m_instance==NULL) {
        m_instance = new GPUIOSBufferCache();
    }
    return m_instance;
}

GPUFrameBuffer* GPUIOSBufferCache::newFrameBuffer(int width, int height, GPUTextureOption_t option, bool only_texture){
    return new GPUIOSFrameBuffer(width, height, option, only_texture);
}

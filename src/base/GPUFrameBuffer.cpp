
/**
 * file :	GPUBuffer.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:12
 * func : 
 * history:
 */

#include "GPUFrameBuffer.h"

#pragma --mark "GPUFrameBuffer"
GPUFrameBuffer::GPUFrameBuffer(gpu_size_t size, bool only_texture)
{
    init(size.width, size.height, GPUFrameBuffer::defaultTextureOption(), only_texture);
}

GPUFrameBuffer::GPUFrameBuffer(int width, int height, bool only_texture){
    init(width, height, GPUFrameBuffer::defaultTextureOption(), only_texture);
}

GPUFrameBuffer::GPUFrameBuffer(int width, int height, GPUTextureOption_t option, bool only_texture){
    init(width, height, option, only_texture);
}

GPUFrameBuffer::GPUFrameBuffer(int width, int height, gpu_buffer_t fb, bool only_texture){
    init(width, height, GPUFrameBuffer::defaultTextureOption(fb), only_texture);
}

GPUFrameBuffer::GPUFrameBuffer(int width, int height, GLuint texture, gpu_buffer_t fb):
m_width(width),
m_height(height){
    m_texture = texture;
    m_outtexture = true;
    m_framebuffer = 0;
    m_renderbuffer = 0;
    m_referencecount = 0;
    m_is_reference = true;
    m_texture_option = GPUFrameBuffer::defaultTextureOption(fb);
}

void GPUFrameBuffer::init(int width, int height, GPUTextureOption_t option, bool only_texture){
    m_width = width;
    m_height = height;
    m_referencecount = 0;
    m_texture_option = option;
    m_is_reference = true;
    
    if (only_texture) {
        generateTexture();
        m_framebuffer = 0;
        m_renderbuffer = 0;
    }
    else{
        generateFrameBuffer();
    }
}

GPUTextureOption_t GPUFrameBuffer::defaultTextureOption(gpu_buffer_t fb){
    GPUTextureOption_t defaultTextureOptions;
    defaultTextureOptions.min_filter = GL_LINEAR;
    defaultTextureOptions.mag_filter = GL_LINEAR;
    defaultTextureOptions.wrap_s = GL_CLAMP_TO_EDGE;
    defaultTextureOptions.wrap_t = GL_CLAMP_TO_EDGE;
    defaultTextureOptions.internal_format = GL_RGBA;
    defaultTextureOptions.format = GL_RGBA;
    defaultTextureOptions.type = GL_UNSIGNED_BYTE;
    defaultTextureOptions.pixel = 4;
	defaultTextureOptions.fb = fb;

	switch(fb) {
	case GPUFB_LUMA:	defaultTextureOptions.internal_format =
						defaultTextureOptions.format = GL_LUMINANCE;
						defaultTextureOptions.pixel = 1;
						break;
	case GPUFB_UV:		defaultTextureOptions.internal_format =
						defaultTextureOptions.format = GL_LUMINANCE_ALPHA;
						defaultTextureOptions.pixel = 2;
						break;
	case GPUFB_XYZW:	defaultTextureOptions.min_filter =
						defaultTextureOptions.mag_filter = GL_NEAREST;
						break;
	case GPUFB_FLOAT:	defaultTextureOptions.min_filter =
						defaultTextureOptions.mag_filter = GL_NEAREST;
						defaultTextureOptions.internal_format =
						defaultTextureOptions.format = GL_LUMINANCE;
						defaultTextureOptions.type = GL_FLOAT;
						break;
	default:;
	}
	
    return defaultTextureOptions;
}

void GPUFrameBuffer::generateTexture(){
    GPUContext::shareInstance()->makeCurrent();
    m_outtexture = false;
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &m_texture);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    // fbo过程不能设置filter，在当做sample之前设置
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_texture_option.min_filter);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_texture_option.mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_texture_option.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_texture_option.wrap_t);
}

void GPUFrameBuffer::generateFrameBuffer(){
    GPUContext::shareInstance()->makeCurrent();
    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    
    generateTexture();
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glGenRenderbuffers(1, &m_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, m_width, m_height);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderbuffer);
    
    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
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
    
    //glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GPUFrameBuffer::setData(void *data){
    GPUContext::shareInstance()->makeCurrent();
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, m_width, m_height,0,GL_RGBA,GL_UNSIGNED_BYTE, data);//装载贴图
    glGenerateMipmap(GL_TEXTURE_2D);//生成贴图所有必要的级别
}

void GPUFrameBuffer::activeBuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    glViewport(0, 0, (int)m_width, (int)m_height);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}
void GPUFrameBuffer::activeTexture(GLuint texture_unit){
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    // fbo不能设置filter，作为sample之前设置
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_texture_option.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_texture_option.mag_filter);
}

void GPUFrameBuffer::destroy(){
    GPUContext::shareInstance()->makeCurrent();

    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
        glDeleteRenderbuffers(1, &m_renderbuffer);
        m_framebuffer = 0;
    }
    if (!m_outtexture) {
        glDeleteTextures(1, &m_texture);
    }
}

bool GPUFrameBuffer::lock(){
    m_referencecount++;
    return true;
}

bool GPUFrameBuffer::unlock(){
    if (m_is_reference && m_referencecount<=0)
    {
        err_log("framebuffer[%d] unlock error! referencecount[%d]", m_texture, m_referencecount);
        return false;
    }
    
    if (m_is_reference) {
        m_referencecount--;
    }
    
    // err_log("texture:%d reference:%d", m_texture, m_referencecount);
    return true;
}

bool GPUFrameBuffer::idle(){
    return m_referencecount==0;
}

void GPUFrameBuffer::disableReference(){
    m_is_reference = false;
}
void GPUFrameBuffer::release(){
    m_is_reference = true;
    m_referencecount = 0;
}

#pragma --mark "GPUBufferCache"
GPUBufferCache* GPUBufferCache::m_instance = NULL;

GPUBufferCache* GPUBufferCache::shareInstance(){
    if (m_instance==NULL)
    {
        m_instance = new GPUBufferCache();
    }
    return m_instance;
}

void GPUBufferCache::destroyInstance(){
    if (m_instance!=NULL)
    {
        m_instance->clear();
        delete m_instance;
        m_instance = NULL;
    }
}

GPUBufferCache::GPUBufferCache(){}

GPUFrameBuffer* GPUBufferCache::getFrameBuffer(int width, int height, gpu_buffer_t fb, bool only_texture){
    int i;
    GPUFrameBuffer* buffer=NULL;

	int n = m_bufferlist.size();
    for (i = 0; i < n; ++i)
    {
        buffer = m_bufferlist[i];
        // 类型一致
        if (buffer->m_width==width && buffer->m_height==height // 长宽一致
			&& (buffer->m_texture_option.fb == (int)fb)		///	格式参数相同
            && (only_texture==(buffer->m_framebuffer==0))   // 是否fbo的类型一致
            && buffer->idle())  // 是否空闲
        {
            return buffer;
        }
    }
    
    if (i>=m_bufferlist.size())
    {
        buffer = newFrameBuffer(width, height, GPUFrameBuffer::defaultTextureOption(), only_texture);
        m_bufferlist.push_back(buffer);
    }

    return buffer;
}

GPUFrameBuffer* GPUBufferCache::newFrameBuffer(int width, int height, GPUTextureOption_t option, bool only_texture){
    return new GPUFrameBuffer(width, height, option, only_texture);
}

GPUFrameBuffer* GPUBufferCache::getFrameBuffer(int width, int height, bool only_texture){
    return getFrameBuffer(width, height, GPUFB_RGBA, only_texture);
}

GPUFrameBuffer* GPUBufferCache::getFrameBuffer(gpu_size_t size, bool only_texture){
    return getFrameBuffer(size.width, size.height, GPUFB_RGBA, only_texture);
}

bool GPUBufferCache::setFrameBuffer(GPUFrameBuffer* buffer){
    m_bufferlist.push_back(buffer);
    return true;
}

void GPUBufferCache::clear(){
    GPUContext* context = GPUContext::shareInstance();
    context->makeCurrent();
    vector<GPUFrameBuffer*>::iterator it = m_bufferlist.begin();
    for (; it!=m_bufferlist.end(); it++) {
        delete *it;
    }
    m_bufferlist.clear();
}

void GPUBufferCache::purge(){
    vector<GPUFrameBuffer*>::iterator it = m_bufferlist.end();
    for (; it!=m_bufferlist.begin(); it--) {
        if ((*it)->idle()) {
            delete *it;
            m_bufferlist.erase(it);
        }
    }
}

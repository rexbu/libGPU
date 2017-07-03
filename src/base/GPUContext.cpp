/**
 * file :	GPUContext.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:30
 * func : 
 * history:
 */
#include "GPUContext.h"
#include "GPUProgram.h"
#include "GPUFrameBuffer.h"

GPUContext* GPUContext::m_instance = NULL;
bool GPUContext::m_has_context = true;

GPUContext* GPUContext::shareInstance(){
    if (m_instance==NULL) {
        m_instance = new GPUContext();
        if (m_has_context) {
            m_instance->createContext();
        }
    }
    return m_instance;
}

void GPUContext::destroyInstance(){
    if (m_instance!=NULL)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

GPUContext::GPUContext(){
	m_current_program = NULL;
    pthread_mutex_init(&m_lock, NULL);

    m_has_context = true;
    m_gpu_context = NULL;
    
#ifdef __ANDROID__
    m_surfacewidth = 1080;
    m_surfaceheight = 1920;
#endif
}

void GPUContext::setActiveProgram(GPUProgram* program){
    makeCurrent();
	if(program != m_current_program){
		program->use();
        m_current_program = program;
    }
}

int GPUContext::maximumTextureSize(){
    GLint maxTextureSize = 0;
    
    makeCurrent();
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    
    return maxTextureSize;
}

gpu_size_t GPUContext::sizeFitsTextureMaxSize(gpu_size_t inputSize){
    int maxTextureSize = maximumTextureSize();
    if ( (inputSize.width < maxTextureSize) && (inputSize.height < maxTextureSize) )
    {
        return inputSize;
    }
    
    gpu_size_t adjustedSize;
    if (inputSize.width > inputSize.height)
    {
        adjustedSize.width = maxTextureSize;
        adjustedSize.height = (maxTextureSize*1.0 / inputSize.width) * inputSize.height;
    }
    else
    {
        adjustedSize.height = maxTextureSize;
        adjustedSize.width = (maxTextureSize*1.0 / inputSize.height) * inputSize.width;
    }
    
    return adjustedSize;
}

int GPUContext::glContextLock(){
    return pthread_mutex_lock(&m_lock);
}
int GPUContext::glContextUnlock(){
    return pthread_mutex_unlock(&m_lock);
}

void GPUCheckGlError(const char* op, bool log, bool lock) {
    GPUContext* context;
    if (lock) {
        context = GPUContext::shareInstance();
        context->glContextLock();
    }
    
    for (GLint error = glGetError(); error!=GL_NO_ERROR; error = glGetError()) {
        if (log)
        {
            err_log("after %s() glError (%x)", op, error);    
        }
    }
    if (lock) {
        context->glContextUnlock();
    }
}

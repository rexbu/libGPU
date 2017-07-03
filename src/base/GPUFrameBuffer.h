/**
 * file :	GPUBuffer.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:12
 * func : 
 * history:
 */

#ifndef	__GPU_BUFFER_H_
#define	__GPU_BUFFER_H_
#include <vector>
#include "GL.h"
#include "GPUContext.h"

using namespace std;
typedef struct GPUTextureOption_t {
    GLenum min_filter;
    GLenum mag_filter;
    GLenum wrap_s;
    GLenum wrap_t;
    GLenum internal_format;
    GLenum format;
    GLenum type;
    int pixel;
	int fb;
} GPUTextureOption_t;

class GPUFrameBuffer{
public:
    GPUFrameBuffer(gpu_size_t size, bool only_texture = true);
    GPUFrameBuffer(int width, int height, bool only_texture = true);
    GPUFrameBuffer(int width, int height, GPUTextureOption_t option, bool only_texture = true);
    GPUFrameBuffer(int width, int height, gpu_buffer_t fb, bool only_texture = true);
    GPUFrameBuffer(int width, int height, GLuint texture, gpu_buffer_t fb = GPUFB_RGBA);
    // 默认构造函数只用于GPUIOSFrameBuffer的初始化，不能用于其他地方
    GPUFrameBuffer(){
        m_width = m_height = m_texture = m_framebuffer = m_referencecount = 0;
        m_is_reference = true;
    }
    
    ~GPUFrameBuffer(){
        destroy();
    }
    
    virtual void generateTexture();
    virtual void generateFrameBuffer();
    void setData(void* data);
    void activeBuffer();
    /// 设置活动纹理单元
    void activeTexture(GLuint texture_unit);

    bool lock();
    bool idle();
    bool unlock();
    void disableReference();    // 不再参加统计计数
    void release();
    
    static GPUTextureOption_t defaultTextureOption(gpu_buffer_t fb = GPUFB_RGBA);
protected:
    void init(int width, int height, GPUTextureOption_t option, bool only_texture = true);
    void destroy();
    
public:
    GPUTextureOption_t m_texture_option;
    GLuint      m_framebuffer;
    GLuint      m_texture;
    GLuint      m_renderbuffer;
    bool        m_outtexture;   // 是否是外部传入texture，外部传入destroy不释放，谁创建谁管理
    int         m_width;
    int         m_height;
    // 被使用次数
    int         m_referencecount;
    bool        m_is_reference;
};

#define GPU_FRAMEBUFFER_NUM 1024
class GPUBufferCache{
public:
    static GPUBufferCache* shareInstance();
    static void destroyInstance();
    GPUBufferCache();
    GPUFrameBuffer* getFrameBuffer(int width, int height, gpu_buffer_t fb, bool only_texture = true);
    GPUFrameBuffer* getFrameBuffer(int width, int height, bool only_texture = true);
    GPUFrameBuffer* getFrameBuffer(gpu_size_t size, bool only_texture = true);
    /**
     * 新建FrameBuffer，子函数可能复写，在IOS中创建方式不一样
     */
    virtual GPUFrameBuffer* newFrameBuffer(int width, int height, GPUTextureOption_t option, bool only_texture=true);
    bool setFrameBuffer(GPUFrameBuffer* frame);
    // 清空framebuffer
    void clear();
    // 将不用的framebuffer释放
    void purge();

protected:
    static GPUBufferCache* m_instance;
    
    vector<GPUFrameBuffer*> m_bufferlist;
};

#endif

/**
 * file :	GPUFilter.h
 * author :	Rex
 * create :	2016-04-27 19:11
 * func : 
 * history:
 */

#ifndef	__GPU_FILTER_H_
#define	__GPU_FILTER_H_

#include <iostream>
#include <vector>
#include <string>
#include "GPUContext.h"
#include "GPUInput.h"
#include "GPUOutput.h"
#include "GPUProgram.h"
#include "GPUVertexBuffer.h"
#include "GPUFrameBuffer.h"

class GPUFilter: public GPUOutput, public GPUInput {
public:
    GPUFilter(bool compile = true, const char* name = NULL); // compile，是否需要编译。GPUGroupFilter不编译
    GPUFilter(const char* fragment, int inputs=1, const char* name=NULL);
    GPUFilter(const char* vertext, const char* fragment, int inputs=1);
    ~GPUFilter();
	
///	void setNextFilter(GPUDualport *filter);
///	void removeNextFilter(GPUDualport *filter);

    /// 设置输入数量，也即texture数量
    virtual void setInputs(int inputs);
	virtual void newFrame();
    virtual void render();
    
    /// 设置填充并计算裁剪
    virtual void setFillMode(gpu_fill_mode_t fillmode);
    // 设置输出重新计算裁剪
	virtual void setOutputSize(uint32_t width, uint32_t height);
    
    // 修改shader
    virtual void changeShader(const char* fragment, const char* vertex = NULL);
    virtual void setExtraParameter(float p){}
    
    // 设置输出framebuffer，如果设置了，则不再从cache中获取buffer
    void setOutputFrameBuffer(GPUFrameBuffer* buffer){ m_special_outbuffer = buffer;}
    void setFloat(const char* name, GLfloat val);
    void setFloat(const char* name, GLfloat* val, int num);
    void setFloatv(const char* name, GLfloat* val, int num);
    void setMatrix(const char* name, GLfloat* val, int num);
    void setInteger(const char* name, int i);
    void enableAttribArray(const char* name);

protected:
    virtual void init();
    virtual void initShader();
    void initParams();
    
    void activeOutFrameBuffer();
    virtual gpu_size_t sizeOfFBO();
    /// 更新帧尺寸并重新计算裁剪
    virtual void setFrameSize(uint32_t width, uint32_t height);
    // 计算裁剪比例，更新顶点
    void calAspectRatio();

    static const GLfloat* coordinatesRotation(gpu_rotation_t mode);

public:
    static const char* g_vertext_shader[];
    static const char* g_fragment_shader;
    static const GLfloat g_vertices[];

///	GPUFrameBuffer* m_firstbuffer;

protected:
    GPUProgram* m_program;
    GLuint      m_position;
    GLuint      m_input_texture;
    GLuint      m_input_coordinate;
    
    std::vector<GLint> m_input_textures;
    std::vector<GLint> m_input_coordinates;

    std::vector<GPUVertexBuffer*> m_coordinate_buffers;

    // 顶点，涉及裁剪
    std::vector<GLfloat> m_vertices;
    // 填充方式
    gpu_fill_mode_t     m_fill_mode;
    
    GPUFrameBuffer*     m_special_outbuffer;
    std::string m_filter_name;  // 用于debug
};

#endif

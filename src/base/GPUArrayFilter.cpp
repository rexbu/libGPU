/**
 * file :	GPUArrayFilter.cpp
 * author :	Rex
 * create :	2017-11-26 13:17
 * func : 
 * history:
 */

#include "GPUArrayFilter.h"

static const char* g_array_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    const int texture_count = %d;
    uniform sampler2D inputImageTexture[texture_count];
    void main()
    {
        gl_FragColor = texture2D(inputImageTexture[0], textureCoordinate);
    }
);

GPUArrayFilter::GPUArrayFilter(bool compile, const char* name):
GPUFilter(compile, name){
}

GPUArrayFilter::GPUArrayFilter(const char* fragment, int inputs, const char* name):
GPUFilter(false, name)
{
    m_inputs = inputs;
    m_program = new GPUProgram(g_vertext_shader[0], fragment, m_filter_name.c_str());
    init();
}

GPUArrayFilter::GPUArrayFilter(const char* vertext, const char* fragment, int inputs):
GPUFilter(false){
    m_inputs = inputs;
    m_program = new GPUProgram(vertext, fragment, m_filter_name.c_str());
    init();
}

void GPUArrayFilter::initShader(){
    if (m_program==NULL)
    {
        err_log("Program create Error!");
        return;
    }
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    
    setInputs(m_inputs);
    
    m_position = m_program->attributeIndex("position");
    m_input_coordinate = m_program->attributeIndex("inputTextureCoordinate");
    // 默认纹理坐标
    m_coordinates[0] = -1;
    
    // 目前fragment最多支持16个纹理, setInputs函数已经申请了m_input_textures空间
    for (int i=0; i<m_inputs; i++) {
        m_input_textures[i] = i+10;
    }
    m_program->setUniformsIndex("inputImageTexture", &m_input_textures[0], m_inputs);
    m_input_texture = m_input_textures[0];
    glEnableVertexAttribArray(m_position);
    glEnableVertexAttribArray(m_input_coordinate);
    
    m_input_coordinates[0] = m_input_coordinate;
    
    m_program->link();
    context->glContextUnlock();
    GPUCheckGlError("Filter init");
}

void GPUArrayFilter::render(){
#if DEBUG_FILTER_NAME
    err_log("filter name: %s texture: %d", m_filter_name.c_str(), m_input_buffers[0]->m_texture);
#endif
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();   // 加锁，防止此时设置参数
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    
    context->setActiveProgram(m_program);
    activeOutFrameBuffer();
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    for (int i=0; i<m_inputs; i++) {
        m_input_buffers[i]->activeTexture(GL_TEXTURE0+i);
        glUniform1i(m_input_textures[i], 0+i);
    }
    
    m_coordinate_buffers[0] = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    if (m_coordinates[0]==-1) {
        memcpy(m_coordinates, GPUFilter::coordinatesRotation(m_rotation), sizeof(GLfloat)*8);
    }
    m_coordinate_buffers[0]->activeBuffer(m_input_coordinate, m_coordinates);
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    // vbo
    GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    vertex_buffer->activeBuffer(m_position, &m_vertices[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();
    m_outbuffer->unactive();
    
    vertex_buffer->disableBuffer(m_position);
    vertex_buffer->unLock();
    m_coordinate_buffers[0]->disableBuffer(m_input_coordinates[0]);
    m_coordinate_buffers[0]->unLock();
    
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    context->glContextUnlock();
}

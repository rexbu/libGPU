/**
 * file :	GPUFilter.cpp
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-04-27 19:12
 * func : 
 * history:
 */
#include "GPUFilter.h"
#include "GPUVertexBuffer.h"

// const char* GPUFilter::g_vertext_shader = SHADER_STRING(
// 	attribute vec4 position;
// 	attribute vec4 inputTextureCoordinate;
// 	varying vec2 textureCoordinate;
// 	void main()
// 	{
// 	    gl_Position = position;
// 	    textureCoordinate = inputTextureCoordinate.xy;
// 	}
// );

const char* GPUFilter::g_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    void main()
    {
        gl_FragColor = texture2D(inputImageTexture, textureCoordinate);
    }
);

const GLfloat GPUFilter::g_vertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f,  1.0f,
};

GPUFilter::GPUFilter(bool compile, const char* name){
    if (name==NULL) {
        m_filter_name = "GPUFilter";
    }
    else{
        m_filter_name = name;
    }

    if (compile) {
        m_program = new GPUProgram(g_vertext_shader[0], g_fragment_shader, m_filter_name.c_str());
        init();
    }
    else{
        m_program = NULL;
        initParams();
    }
}

GPUFilter::GPUFilter(const char* fragment, int inputs, const char* name):
GPUInput(inputs){
    if (name==NULL) {
        m_filter_name = "GPUFilter";
    }
    else{
        m_filter_name = name;
    }

	m_program = new GPUProgram(g_vertext_shader[inputs-1], fragment, m_filter_name.c_str());
    init();
}

GPUFilter::GPUFilter(const char* vertext, const char* fragment, int inputs):
GPUInput(inputs){
	m_program = new GPUProgram(vertext, fragment);
    init();
}

GPUFilter::~GPUFilter()
{
    if (m_program)
    {
		delete m_program;
        m_program = NULL;
    }
}

void GPUFilter::changeShader(const char* fragment, const char* vertex){
    if (fragment==NULL) {
        return;
    }
    const char* ver = vertex;
    if (ver==NULL) {
        ver = g_vertext_shader[0];
    }
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    
    context->makeCurrent();
    delete m_program;
    context->glContextUnlock();
    
    m_program = new GPUProgram(ver, fragment);
    
    initShader();
}

void GPUFilter::init(){
   initParams();
   initShader();
}

void GPUFilter::initParams(){
    m_out_width = 0;
    m_out_height = 0;
    m_frame_width = 0;
    m_frame_height = 0;
    m_outbuffer = NULL;
    m_special_outbuffer = NULL;
    m_rotation = GPUNoRotation;
    // 初始化顶点数据
    m_vertices.resize(8);
    memcpy(&m_vertices[0], g_vertices, sizeof(GLfloat)*8);
    
    setFillMode(GPUFillModeStretch);
}
void GPUFilter::initShader(){
    if (m_program==NULL)
    {
        err_log("Program create Error!");
        return;
    }
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    
    m_position = m_program->attributeIndex("position");
    m_input_coordinate = m_program->attributeIndex("inputTextureCoordinate");
    m_input_texture = m_program->uniformIndex("inputImageTexture");
    glEnableVertexAttribArray(m_position);
    glEnableVertexAttribArray(m_input_coordinate);

    setInputs(m_inputs);
    m_input_coordinates[0] = m_input_coordinate;
    m_input_textures[0] = m_input_texture;
    // 遍历多texture
    char name[1024];
    for (int i = 1; i < m_inputs; ++i){
        sprintf(name, "inputTextureCoordinate%d", i+1);
        m_input_coordinates[i] = m_program->attributeIndex(name);
        if (m_input_coordinates[i] >= 0){
            glEnableVertexAttribArray(m_input_coordinates[i]);
        }
        else{
            err_log("Filter[%s] get coordinate%d error", m_filter_name.c_str(), i+1);
        }
        
        sprintf(name, "inputImageTexture%d", i+1);
        m_input_textures[i] = m_program->uniformIndex(name);
        if (m_input_textures[i]<0)
        {
            err_log("Filter[%s] get textures%d error", m_filter_name.c_str(), i);
        }
    }

    m_program->link();
    context->glContextUnlock();
    GPUCheckGlError("Filter init");
}

void GPUFilter::setInputs(int inputs){
    GPUInput::setInputs(inputs);
    m_input_coordinates.resize(inputs);
    m_input_textures.resize(inputs);
    m_coordinate_buffers.resize(inputs);
    for (int i = 0; i < inputs; ++i)
    {
        m_input_coordinates[i] = -1;
        m_input_textures[i] = -1;
        m_coordinate_buffers[i] = NULL;
    }
}

void GPUFilter::render(){
#if DEBUG_FILTER_NAME
    err_log("filter name: %s texture: %d", m_filter_name.c_str(), m_input_buffers[0]->m_texture);
#endif
    
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();   // 加锁，防止此时设置参数
    GPUCheckGlError(m_filter_name.c_str(), true, false);
    
    context->setActiveProgram(m_program);
    activeOutFrameBuffer();

    for (int i=0; i<m_inputs; i++) {
        m_input_buffers[i]->activeTexture(GL_TEXTURE0+i);
        glUniform1i(m_input_textures[i], 0+i);

        m_coordinate_buffers[i] = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
        if (m_input_coordinates[i]>=0) {
            m_coordinate_buffers[i]->activeBuffer(m_input_coordinates[i], GPUFilter::coordinatesRotation(m_rotation));
        }
        else{
            //err_log("filter %s coordinate lost: %d", m_filter_name.c_str(), i);
        }
    }
    // vbo
    GPUVertexBuffer* vertex_buffer = GPUVertexBufferCache::shareInstance()->getVertexBuffer();
    vertex_buffer->activeBuffer(m_position, &m_vertices[0]);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();
    
    vertex_buffer->disableBuffer(m_position);
    for (int i = 0; i < m_inputs; ++i)
    {
        if (m_input_coordinates[i]>=0){
            m_coordinate_buffers[i]->disableBuffer(m_input_coordinates[i]);
        }
        m_coordinate_buffers[i]->unLock();
    }
    
    vertex_buffer->unLock();
    
    context->glContextUnlock();
}

void GPUFilter::newFrame(){
    if (ready())
    {
        if (m_disable) {
            m_outbuffer = m_input_buffers[0];
            unlockInputFrameBuffers();
            informTargets();
        }
        else{
            render();
            m_special_outbuffer = NULL;
            
            // render后的回调
            if (m_complete!=NULL) {
                m_complete(this, m_para);
            }
            
            unlockInputFrameBuffers();
            informTargets();
        }
    }
}

void GPUFilter::activeOutFrameBuffer(){
    if (m_special_outbuffer==NULL) {
        m_outbuffer = GPUBufferCache::shareInstance()->getFrameBuffer(sizeOfFBO(), false);
        m_outbuffer->activeBuffer();
        
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
    else{
        m_outbuffer = m_special_outbuffer;
        m_outbuffer->activeBuffer();
    }
}

gpu_size_t GPUFilter::sizeOfFBO(){
    gpu_size_t size = {0, 0};
    if (m_out_width!=0 && m_out_height!=0) {
        size.width = m_out_width;
        size.height = m_out_height;
	}
    else{
        size.width =m_frame_width;
        size.height = m_frame_height;
    }

    return size;
}

void GPUFilter::setOutputSize(uint32_t width, uint32_t height){
    GPUOutput::setOutputSize(width, height);
    calAspectRatio();
}

void GPUFilter::setFillMode(gpu_fill_mode_t mode){
    m_fill_mode = mode;
    calAspectRatio();
}

void GPUFilter::setFrameSize(uint32_t width, uint32_t height){
    GPUInput::setFrameSize(width, height);
    calAspectRatio();
}

void GPUFilter::calAspectRatio(){
    switch(m_fill_mode){// 用黑框填充
        case GPUFillModePreserveAspectRatio:
            break;
            // 按照比例裁剪
        case GPUFillModePreserveAspectRatioAndFill:
            if (m_out_width==0||m_out_height==0||m_frame_width==0||m_frame_height==0)
            {
                memcpy(&m_vertices[0], g_vertices, sizeof(GLfloat)*8);
            }
            else{
                float wh_ratio = m_out_width*1.0/m_out_height;
                float hw_ratio = m_out_height*1.0/m_out_width;
                if (wh_ratio > m_frame_width*1.0/m_frame_height) {
                    float ratioy = m_frame_width*hw_ratio;
                    m_vertices[0] = -1.0;
                    m_vertices[1] = (-1.0)*m_frame_height/ratioy;
                    m_vertices[2] = 1.0;
                    m_vertices[3] = m_vertices[1];
                    m_vertices[4] = -1.0;
                    m_vertices[5] = m_frame_height/ratioy;
                    m_vertices[6] = 1.0;
                    m_vertices[7] = m_vertices[5];
                }
                else{
                    float ratiox = m_frame_height*wh_ratio;
                    m_vertices[0] = (-1.0)*m_frame_width/ratiox;
                    m_vertices[1] = -1.0;
                    m_vertices[2] = m_frame_width/ratiox;
                    m_vertices[3] = -1.0;
                    m_vertices[4] = m_vertices[0];
                    m_vertices[5] = 1.0;
                    m_vertices[6] = m_vertices[2];
                    m_vertices[7] = 1.0;
                }
            }
            break;
        case GPUFillModeStretch:
        default:
            memcpy(&m_vertices[0], g_vertices, sizeof(GLfloat)*8);
            break;
    }
    // err_log("filter: %s, fill: %d", m_filter_name.c_str(), m_fill_mode);
    // err_log("vertex: %0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f", 
    //         m_vertices[0],m_vertices[1],m_vertices[2],m_vertices[3],m_vertices[4],m_vertices[5],m_vertices[6],m_vertices[7]);
}

const GLfloat* GPUFilter::coordinatesRotation(gpu_rotation_t mode)
{
    static const GLfloat noRotationTextureCoordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };
    
    static const GLfloat rotateLeftTextureCoordinates[] = {
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };
    
    static const GLfloat rotateRightTextureCoordinates[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    
    static const GLfloat verticalFlipTextureCoordinates[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };
    
    static const GLfloat horizontalFlipTextureCoordinates[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f,  1.0f,
        0.0f,  1.0f,
    };
    
    static const GLfloat rotateRightVerticalFlipTextureCoordinates[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotateRightHorizontalFlipTextureCoordinates[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };
    static const GLfloat rotateLeftHorizontalFlipTextureCoordinates[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotate180TextureCoordinates[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };
    
    switch(mode)
    {
        case GPUNoRotation: return noRotationTextureCoordinates;
        case GPURotateLeft: return rotateLeftTextureCoordinates;
        case GPURotateRight: return rotateRightTextureCoordinates;
        case GPUFlipVertical: return verticalFlipTextureCoordinates;
        case GPUFlipHorizonal: return horizontalFlipTextureCoordinates;
        case GPURotateRightFlipVertical: return rotateRightVerticalFlipTextureCoordinates;
        case GPURotateRightFlipHorizontal: return rotateRightHorizontalFlipTextureCoordinates;
        case GPURotate180: return rotate180TextureCoordinates;
		default: return noRotationTextureCoordinates;
    }
}
/*
void GPUFilter::setNextFilter(GPUDualport* filter){
    for (int i=0; i < targetsCount(); i++) {
		int loc;
		GPUInput *f = getTarget(i, loc);
        filter->addTarget(f, loc);
    }
    removeAllTargets();
    addTarget(filter);
}

void GPUFilter::removeNextFilter(GPUDualport *filter){
    removeTarget(filter);
    for (int i=0; i<filter->targetsCount(); i++) {
		int loc;
		GPUInput *f = filter->getTarget(i, loc);
        addTarget(f, loc);
    }
    filter->removeAllTargets();
}
*/
void GPUFilter::enableAttribArray(const char* name){
    GLuint index = m_program->attributeIndex(name);
    glEnableVertexAttribArray(index);
}

void GPUFilter::setFloat(const char *name, GLfloat val){
    m_program->setFloat(name, val);
}
void GPUFilter::setFloat(const char *name, GLfloat *val, int num){
    m_program->setFloat(name, val, num);
}
void GPUFilter::setFloatv(const char* name, GLfloat* val, int num){
    m_program->setFloatv(name, val, num);
}

void GPUFilter::setMatrix(const char *name, GLfloat *val, int num){
    m_program->setMatrix(name, val, num);
}

void GPUFilter::setInteger(const char* name, int i){
    m_program->setInteger(name, i);
}

const char* GPUFilter::g_vertext_shader[] = {
SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
    }
),
SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;
    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
        textureCoordinate2 = inputTextureCoordinate2.xy;
    }
),
SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;
    attribute vec4 inputTextureCoordinate3;
    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;
    varying vec2 textureCoordinate3;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
        textureCoordinate2 = inputTextureCoordinate2.xy;
        textureCoordinate3 = inputTextureCoordinate3.xy;
    }
),
SHADER_STRING(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    attribute vec4 inputTextureCoordinate2;
    attribute vec4 inputTextureCoordinate3;
    attribute vec4 inputTextureCoordinate4;
    varying vec2 textureCoordinate;
    varying vec2 textureCoordinate2;
    varying vec2 textureCoordinate3;
    varying vec2 textureCoordinate4;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
        textureCoordinate2 = inputTextureCoordinate2.xy;
        textureCoordinate3 = inputTextureCoordinate3.xy;
        textureCoordinate4 = inputTextureCoordinate4.xy;
    }
),
SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
 }
),
SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;
 attribute vec4 inputTextureCoordinate6;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 varying vec2 textureCoordinate6;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
     textureCoordinate6 = inputTextureCoordinate6.xy;
 }
),
SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;
 attribute vec4 inputTextureCoordinate6;
 attribute vec4 inputTextureCoordinate7;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 varying vec2 textureCoordinate6;
 varying vec2 textureCoordinate7;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
     textureCoordinate6 = inputTextureCoordinate6.xy;
     textureCoordinate7 = inputTextureCoordinate7.xy;
 }
),
SHADER_STRING(
 attribute vec4 position;
 attribute vec4 inputTextureCoordinate;
 attribute vec4 inputTextureCoordinate2;
 attribute vec4 inputTextureCoordinate3;
 attribute vec4 inputTextureCoordinate4;
 attribute vec4 inputTextureCoordinate5;
 attribute vec4 inputTextureCoordinate6;
 attribute vec4 inputTextureCoordinate7;
 attribute vec4 inputTextureCoordinate8;

 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;
 varying vec2 textureCoordinate3;
 varying vec2 textureCoordinate4;
 varying vec2 textureCoordinate5;
 varying vec2 textureCoordinate6;
 varying vec2 textureCoordinate7;
 varying vec2 textureCoordinate8;
 void main()
 {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate2 = inputTextureCoordinate2.xy;
     textureCoordinate3 = inputTextureCoordinate3.xy;
     textureCoordinate4 = inputTextureCoordinate4.xy;
     textureCoordinate5 = inputTextureCoordinate5.xy;
     textureCoordinate6 = inputTextureCoordinate6.xy;
     textureCoordinate7 = inputTextureCoordinate7.xy;
     textureCoordinate8 = inputTextureCoordinate8.xy;
 }
),
};

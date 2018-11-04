/**
 * file :	GPULookupFilter.cpp
 * author :	Rex
 * create :	2018-07-14 22:59
 * func : 
 * history:
 */

#include "GPULookupFilter.h"

const static char* g_lookup_fragment_shader = SHADER_STRING(
varying highp vec2 textureCoordinate;
// varying vec2 textureCoordinate;

uniform sampler2D inputImageTexture[2];

uniform float strength;

void main() {
    highp vec4 textureColor = texture2D(inputImageTexture[0], textureCoordinate);
    
    highp float blueColor = textureColor.b * 63.0;
    
    highp vec2 quad1;
    quad1.y = floor(floor(blueColor) / 8.0);
    quad1.x = floor(blueColor) - (quad1.y * 8.0);
    
    highp vec2 quad2;
    quad2.y = floor(ceil(blueColor) /8.0);
    quad2.x = ceil(blueColor) - (quad2.y * 8.0);
    
    highp vec2 texPos1;
    texPos1.x = (quad1.x * 1.0/8.0) + 0.5/512.0 + ((1.0/8.0 - 1.0/512.0) * textureColor.r);
    texPos1.y = (quad1.y * 1.0/8.0) + 0.5/512.0 + ((1.0/8.0 - 1.0/512.0) * textureColor.g);
    
    highp vec2 texPos2;
    texPos2.x = (quad2.x * 1.0/8.0) + 0.5/512.0 + ((1.0/8.0 - 1.0/512.0) * textureColor.r);
    texPos2.y = (quad2.y * 1.0/8.0) + 0.5/512.0 + ((1.0/8.0 - 1.0/512.0) * textureColor.g);
    
    lowp vec4 newColor1 = texture2D(inputImageTexture[1], texPos1);
    lowp vec4 newColor2 = texture2D(inputImageTexture[1], texPos2);
    
    lowp vec4 newColor = mix(newColor1, newColor2, fract(blueColor));
    gl_FragColor = mix(textureColor, vec4(newColor.rgb, textureColor.w), 1.0 - strength);
}

);
GPULookupFilter::GPULookupFilter(const char* path):
GPUFilter(g_lookup_fragment_shader, 2, "Lookup Filter"){
    m_lookup = NULL;
    setLookupImage(path);
}

GPULookupFilter::~GPULookupFilter(){
    if (m_lookup != NULL){
        delete m_lookup;
    }
}
bool GPULookupFilter::setLookupImage(const char* path){
    GPUContext* context = GPUContext::shareInstance();
    context->glContextLock();
    
    if (m_lookup!=NULL) {
        m_lookup->removeAllTargets();
        delete m_lookup;
        m_lookup = NULL;
    }
    
    m_lookup = new GPUPicture(path);
    bool flag = true;
    if(m_lookup->exist()){
        m_lookup->addTarget(this, 1);
    }
    else{
        flag = false;
        delete m_lookup;
        m_lookup = NULL;
    }
    
    context->glContextUnlock();
    // 设置参数有锁
    setExtraParameter(0.0);
    return flag;
}

void GPULookupFilter::setExtraParameter(float p){
    setFloat("strength", p);
}

void GPULookupFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    GPUFilter::setInputFrameBuffer(buffer, location);
    if (m_lookup!=NULL && m_inputs>1 && location==0) {
        m_lookup->processImage();
    }
}

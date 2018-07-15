/**
 * file :	GPULookupFilter.cpp
 * author :	Rex
 * create :	2018-07-14 22:59
 * func : 
 * history:
 */

#include "GPULookupFilter.h"

const static char* g_lookup_fragment_shader = SHADER_STRING(
uniform sampler2D inputImageTexture[2];
uniform int hasLookup;

varying vec2 textureCoordinate;

void main() {
    if (hasLookup == 0) {
        gl_FragColor = texture2D(inputImageTexture[0], textureCoordinate);
        return;
    }
    
    highp vec4 result  = texture2D(inputImageTexture[0], textureCoordinate);
    result = result * vec4(0.9, 0.9, 0.9, 1.0) + vec4(0.05, 0.05, 0.05, 0.0);
    
    // lookup filter
    highp float blueColor = result.b * 63.0;
    highp vec2 quad1;
    quad1.y = floor(floor(blueColor) / 8.0);
    quad1.x = floor(blueColor) - (quad1.y * 8.0);
    
    highp vec2 quad2;
    quad2.y = floor(ceil(blueColor) / 8.0);
    quad2.x = ceil(blueColor) - (quad2.y * 8.0);
    
    highp vec2 texPos1;
    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * result.r);
    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * result.g);
    
    highp vec2 texPos2;
    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * result.r);
    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * result.g);
    
    highp vec4 newColor1 = texture2D(inputImageTexture[1], texPos1);
    highp vec4 newColor2 = texture2D(inputImageTexture[1], texPos2);
    
    highp vec4 newColor = mix(newColor1, newColor2, fract(blueColor));
    result = vec4(newColor.rgb, result.w);
    
    gl_FragColor = result;
}

);
GPULookupFilter::GPULookupFilter():
GPUFilter(g_lookup_fragment_shader, 2, "Lookup Filter"){
    m_lookup = NULL;
}

void GPULookupFilter::setLookupImage(const char* path){
    stopLookup();
    
    m_lookup = new GPUPicture(path);
    if(m_lookup->exist()){
        // 只需要更改texture数量
        GPUInput::setInputs(2);
        setInteger("hasLookup", 1);
        m_lookup->addTarget(this, 1);
    }
    else{
        delete m_lookup;
    }
}

void GPULookupFilter::stopLookup(){
    if (m_lookup) {
        m_lookup->removeAllTargets();
        delete m_lookup;
    }
    m_lookup = NULL;
    setInteger("hasLookup", 0);
    // 只需要更改texture数量
    GPUInput::setInputs(1);
}

void GPULookupFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    GPUFilter::setInputFrameBuffer(buffer, location);
    if (m_lookup!=NULL && m_inputs>1 && location==0) {
        m_lookup->processImage();
    }
}

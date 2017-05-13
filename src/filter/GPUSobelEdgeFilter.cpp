/**
 * file :	GPUSobelEdgeFilter.cpp
 * author :	Rex
 * create :	2016-08-10 20:57
 * func : 
 * history:
 */

#include "GPUSobelEdgeFilter.h"

const static char* g_sobeledge_fragment_shader = SHADER_STRING
(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform mediump float edgeStrength;
 uniform mediump float widthStep;
 uniform mediump float heightStep;
 
 void main()
 {
    mediump float bottomLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, heightStep)).r;
    mediump float topRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, -heightStep)).r;
    mediump float topLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, -heightStep)).r;
    mediump float bottomRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, heightStep)).r;
    mediump float leftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, 0.0)).r;
    mediump float rightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, 0.0)).r;
    mediump float bottomIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep)).r;
    mediump float topIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep)).r;
     
    mediump float h = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0 * bottomIntensity + bottomRightIntensity;
    mediump float v = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;
    
    mediump float mag = length(vec2(h, v))*edgeStrength;
    
    gl_FragColor = vec4(vec3(mag), 1.0);
 }
);

GPUSobelEdgeFilter::GPUSobelEdgeFilter(float strength){
    m_first_filter = new GPULuminanceFilter();
    m_second_filter = new GPUFilter(g_sobeledge_fragment_shader);
    setExtraParameter(strength);
}

void GPUSobelEdgeFilter::setExtraParameter(float p){
    m_second_filter->setFloat("edgeStrength", p);
}

void GPUSobelEdgeFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    GPUTwoPassFilter::setInputFrameBuffer(buffer, location);
    // 如果尺寸发生变化，重新设置尺寸
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
    {
        m_frame_width = buffer->m_width;
        m_frame_height = buffer->m_height;
        m_second_filter->setFloat("widthStep", 1.0/(m_frame_width-1));
        m_second_filter->setFloat("heightStep", 1.0/(m_frame_height-1));
    }
}


const static char* g_sobeldirection_fragment_shader = SHADER_STRING
(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform mediump float edgeStrength;
 uniform mediump float widthStep;
 uniform mediump float heightStep;
 
 void main()
 {
    mediump float bottomLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, heightStep)).r;
    mediump float topRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, -heightStep)).r;
    mediump float topLeftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, -heightStep)).r;
    mediump float bottomRightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, heightStep)).r;
    mediump float leftIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(-widthStep, 0.0)).r;
    mediump float rightIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(widthStep, 0.0)).r;
    mediump float bottomIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, heightStep)).r;
    mediump float topIntensity = texture2D(inputImageTexture, textureCoordinate+vec2(0.0, -heightStep)).r;
     
	mediump vec2 gradientDirection;
	gradientDirection.x = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;
	gradientDirection.y = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0 * bottomIntensity + bottomRightIntensity;

	mediump float gradientMagnitude = length(gradientDirection);
	mediump vec2 normalizedDirection = normalize(gradientDirection);
	normalizedDirection = sign(normalizedDirection) * floor(abs(normalizedDirection) + 0.617316); // Offset by 1-sin(pi/8) to set to 0 if near axis, 1 if away
	normalizedDirection = (normalizedDirection + 1.0) * 0.5; // Place -1.0 - 1.0 within 0 - 1.0

	gl_FragColor = vec4(gradientMagnitude, normalizedDirection.x, normalizedDirection.y, 1.0);
 }
);
GPUSobelDirectionFilter::GPUSobelDirectionFilter(float strength):
GPUFilter(g_sobeldirection_fragment_shader){
    setExtraParameter(strength);
}

void GPUSobelDirectionFilter::setExtraParameter(float p){
    setFloat("edgeStrength", p);
}
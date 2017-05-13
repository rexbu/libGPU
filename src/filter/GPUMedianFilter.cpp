/**
 * file :	GPUMedianFilter.cpp
 * author :	Rex
 * create :	2016-08-05 15:05
 * func : 
 * history:
 */

#include "GPUMedianFilter.h"

const static char* g_median_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;

    uniform mediump float xStep;
    uniform mediump float yStep;
    uniform mediump float distance;
    uniform mediump float step;
    uniform mediump float weight;
    void main()
    {
    	mediump vec4 value = vec4(0.0);
        mediump vec2 stepOffset = vec2(xStep, yStep);
        for(mediump float i = 0.0; i<distance*2.0+1.0; i+=step){
            //value += weight* abs(i-distance) * texture2D(inputImageTexture, textureCoordinate + stepOffset*(i-distance));
            value += weight* texture2D(inputImageTexture, textureCoordinate + stepOffset*(i-distance));
        }
        gl_FragColor = value;
    }
);

GPUMedianFilter::GPUMedianFilter(uint32_t distance):GPUTwoPassFilter(g_median_fragment_shader, g_median_fragment_shader){
	float step = (distance*2.0+1.0)/7;
	float weight = 1.0/7;
	// 最多遍历7个点
	if (step<1)
	{
		step = 1.0;
		weight = 1.0/(2*distance+1);
	}
	m_first_filter->setFloat("distance", distance*1.0);
	m_first_filter->setFloat("step", step);
	m_first_filter->setFloat("weight", weight);
    m_second_filter->setFloat("distance", distance*1.0);
    m_second_filter->setFloat("step", step);
	m_second_filter->setFloat("weight", weight);
}

void GPUMedianFilter::setPixelRadius(uint32_t distance){
    float step = (distance*2.0+1.0)/7;
    float weight = 1.0/7;
    // 最多遍历7个点
    if (step<1)
    {
        step = 1.0;
        weight = 1.0/(2*distance+1);
    }
    m_first_filter->setFloat("distance", distance*1.0);
    m_first_filter->setFloat("step", step);
    m_first_filter->setFloat("weight", weight);
    m_second_filter->setFloat("distance", distance*1.0);
    m_second_filter->setFloat("step", step);
    m_second_filter->setFloat("weight", weight);
}

void GPUMedianFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    GPUTwoPassFilter::setInputFrameBuffer(buffer, location);
    
    // 如果尺寸发生变化，重新设置尺寸
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
    {
        m_frame_width = buffer->m_width;
        m_frame_height = buffer->m_height;
        m_first_filter->setFloat("xStep", 1.0/(m_frame_width-1));
        m_first_filter->setFloat("yStep", 0.0);
        m_second_filter->setFloat("xStep", 0.0);
        m_second_filter->setFloat("yStep", 1.0/(m_frame_height-1));
    }
}

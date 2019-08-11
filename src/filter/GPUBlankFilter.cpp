/**
 * file :	GPUBlankFilter.cpp
 * author :	Rex
 * create :	2018-09-07 22:19
 * func : 
 * history:
 */

#include "GPUBlankFilter.h"

void GPUBlankFilter::setStreamFrameSize(int width, int height){
    if(m_fill_mode != GPUFillModePreserveAspectRatio){
        setFillMode(GPUFillModePreserveAspectRatioAndFill);
    }
    setOutputSize(width, height);
    if(m_border > 0 && m_frame_width>0 && m_frame_height>0){
        float wf = m_border / m_frame_width;
        float hf = m_border / m_frame_height;
        float* v = getVertices();
        v[0] += wf;
        v[1] += hf;
        v[2] -= wf;
        v[3] += hf;
        v[4] += wf;
        v[5] -= hf;
        v[6] -= wf;
        v[7] -= hf;
    }
}

void GPUBlankFilter::setBlank(int border, int r, int g, int b){
    m_border = border;
    if(border==0){
        setFillMode(GPUFillModePreserveAspectRatioAndFill);
        return;
    }
    
    setFillMode(GPUFillModePreserveAspectRatio);
    if (m_frame_width>0 && m_frame_height>0){
        float wf = border*1.0f / m_frame_width;
        float hf = border*1.0f / m_frame_height;
        float* v = getVertices();
        v[0] += wf;
        v[1] += hf;
        v[2] -= wf;
        v[3] += hf;
        v[4] += wf;
        v[5] -= hf;
        v[6] -= wf;
        v[7] -= hf;
    }
    
    setClearColor(r/255.0f, g/255.0f, b/255.0f);
}

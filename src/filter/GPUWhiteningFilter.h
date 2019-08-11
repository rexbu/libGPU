/**
 * file :	GPUWhiteningFilter.h
 * author :	Rex
 * create :	2016-12-06 23:32
 * func : 
 * history:
 */

#ifndef	__GPUWHITENINGFILTER_H_
#define	__GPUWHITENINGFILTER_H_

#include "GPUFilter.h"

class GPUWhiteningFilter: public GPUFilter{
public:
    GPUWhiteningFilter(float s = 0);
    
    void setIdeal(float ideal[4]);
    void setThreshold(float thresh[4]);
    void setStrength(float s);
protected:
    GLfloat m_idealpnts[8];
    GLfloat m_threshold[4];
    float     m_strength;
    
    GLfloat m_rgb2yuv[9];
    GLfloat m_off2yuv[3];
};

#endif

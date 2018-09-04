/**
 * file :	GPUGaussianBlurFilter.h
 * author :	Rex
 * create :	2016-07-13 23:29
 * func :   高斯模糊
 * history:
 */

#ifndef	__GPUGAUSSIANBLURFILTER_H_
#define	__GPUGAUSSIANBLURFILTER_H_

#include "GPUGroupFilter.h"
#include "GPUMedianFilter.h"

class GPUGaussianBlurFilter: public GPUGroupFilter{
public:
    GPUGaussianBlurFilter(uint32_t radius=16, float sigma=2.0);
    
    virtual void setExtraParameter(float sigma);
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    
    void setUnBlurRegion(int x, int y, int radius);
protected:
    char* generateShader(uint32_t radius, float sigma);
    void setUnBlurRegion();
    
    char        m_fragment[10240];
    int         m_pixel_radius;
    int         m_pixel_sigma;
    
    GPUFilter   m_x_filter;
    GPUFilter   m_y_filter;
    
    int         m_unblur_x;
    int         m_unblur_y;
    int         m_unblur_radius;
};

#endif

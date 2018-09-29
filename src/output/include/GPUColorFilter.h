/**
 * file :	GPUColorFilter.h
 * author :	Rex
 * create :	2017-05-26 12:13
 * func : 
 * history:
 */

#ifndef	__GPUCONTRASTFILTER_H_
#define	__GPUCONTRASTFILTER_H_

#include "GPUGroupFilter.h"
#include "GPUGaussianBlurFilter.h"

// 处理曝光、饱和、褪色、对比度
class GPUColorShotFilter: public GPUFilter{
public:
    GPUColorShotFilter();
    
    
};

// 处理模糊、锐化、色温、色调、高光、阴影、暗角
class GPUColorAdjustFilter: public GPUFilter{
public:
    GPUColorAdjustFilter();
    
    // 模糊
};

class GPUColorFilter: public GPUGroupFilter{
public:
    GPUColorFilter();
    
    // 对比度 [-1, 1]
    void setContrast(float p);
    // 曝光度 [-1, 1]
    void setGamma(float p);
    // 饱和度 [-1, 1]
    void setSaturation(float p);
    // 褪色 [0, 1]
    void setFade(float p);
    
    // 设置模糊 [0, 1]
    void setBlur(float p);
    // 设置模糊范围
    void setUnBlurRegion(int x, int y, int radius);
    // 锐化 [0,1]
    void setSharpness(float p);
    // 色温 [-1,1]
    void setTemperature(float p);
    // 色调 [-1,1]
    void setTint(float p);
    // 高光 [0,1]
    void setHighlights(float p);
    // 阴影 [0,1]
    void setShadows(float p);
    // 暗角 [0,1]
    void setVignette(float intensity);

public:
    GPUFilter       m_shot_filter;      // 处理曝光、饱和、褪色、对比度
    GPUFilter       m_adjust_filter;    // 处理模糊、锐化、色温、色调、高光、阴影、暗角
    GPUGaussianBlurFilter m_blur_filter;
};

#endif

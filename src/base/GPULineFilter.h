/**
 * file :	GPULineFilter.h
 * author :	Rex
 * create :	2017-05-19 15:17
 * func : 
 * history:
 */

#ifndef	__GPULINEFILTER_H_
#define	__GPULINEFILTER_H_

#include "GL.h"
#include "GPUFilter.h"

class GPULineFilter:public GPUFilter{
public:
    GPULineFilter(int width=720, int height=1280);
    
    virtual void initShader();
    virtual void render();
    
    // 线段宽度，像素数
    void setLineWidth(uint32_t width);
    void setPoints(gpu_point_t* points, int n);
    void setColors(gpu_colorf_t* color, int n);
    void setColor(gpu_colorf_t* color, int i);
    
    void drawPoint(float x, float y, uint32_t radius, float* color);
    void drawRound(float x, float y, uint32_t radius, float* color);
    
protected:
    GLuint      m_color;
    uint32_t    m_line_width;   // 线条宽度，像素宽度
    
    std::vector<float>      m_points;
    std::vector<float>      m_colors;
    uint32_t                m_vertex_count;
};

#endif

/**
 * file :	GPURawGroup.h
 * author :	Rex
 * create :	2018-11-16 13:20
 * func : 
 * history:
 */

#ifndef	__GPURAWGROUP_H_
#define	__GPURAWGROUP_H_

#include "GPUGroupFilter.h"
#include "GPUZoomFilter.h"
#include "GPUYUVFilter.h"
#include "GPUYUV420Filter.h"

class GPURawGroup:public GPUGroupFilter{
public:
    GPURawGroup();
    void setOutputFormat(gpu_pixel_format_t format);

    GPURawOutput        m_raw_output;
protected:
    gpu_pixel_format_t  m_output_format;

    GPUZoomFilter       m_zoom_filter;

    GPURGBToYUVFilter   m_yuv_filter;
    GPUToYUV420Filter   m_yuv420_filter;
    GPUToNV21Filter     m_nv21_filter;
    GPUToNV12Filter     m_nv12_filter;
};
#endif

/**
 * file :	GPURawGroup.cpp
 * author :	Rex
 * create :	2018-11-16 13:20
 * func : 
 * history:
 */

#include "GPURawGroup.h"

GPURawGroup::GPURawGroup(){
    m_input = &m_zoom_filter;
#ifdef __ANDROID__
    m_yuv_filter.setOutputRotation(GPUFlipVertical);
#endif
    //m_output = &m_raw_output;
}
void GPURawGroup::setOutputFormat(gpu_pixel_format_t format){
    if (format == GPU_UNKNOWN)
    {
        return;
    }

    m_output_format = format;
    m_zoom_filter.removeAllTargets();
    m_yuv_filter.removeAllTargets();

    switch(m_output_format) {
        case GPU_BGRA:
            m_zoom_filter.setOutputFormat(GPU_BGRA);
            err_log("output format bgra");
#ifdef __ANDROID__
            m_zoom_filter.addTarget(&m_raw_output);
#endif
            break;
        case GPU_RGBA:
            m_zoom_filter.setOutputFormat(GPU_RGBA);
            m_zoom_filter.addTarget(&m_raw_output);
            err_log("output format rgba");
            break;
        case GPU_ARGB:
            m_zoom_filter.setOutputFormat(GPU_ARGB);
            m_zoom_filter.addTarget(&m_raw_output);
            err_log("output format argb");
            break;
        case GPU_I420:
            m_zoom_filter.addTarget(&m_yuv_filter);
            m_yuv_filter.addTarget(&m_yuv420_filter);
            m_yuv420_filter.addTarget(&m_raw_output);
            err_log("output format yuv420p");
            break;
        case GPU_NV21:
            m_zoom_filter.addTarget(&m_yuv_filter);
            m_yuv_filter.addTarget(&m_nv21_filter);
            m_nv21_filter.addTarget(&m_raw_output);
            err_log("output format nv21");
            break;
        case GPU_NV12:
            m_zoom_filter.addTarget(&m_yuv_filter);
            m_yuv_filter.addTarget(&m_nv12_filter);
            m_nv12_filter.addTarget(&m_raw_output);
            err_log("output format nv12");
            break;
        default:
            err_log("Error: Not Support Format!");
            break;
    }
}
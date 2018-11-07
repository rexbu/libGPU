/**
 * file :	GPUStreamFrame.h
 * author :	Rex
 * create :	2018-07-10 21:09
 * func : 
 * history:
 */

#ifndef	__GPUSTREAMFRAME_H_
#define	__GPUSTREAMFRAME_H_

#include "GPU.h"

#define GPU_COLOR_CONTRAST_FILTER   0       // 对比度
#define GPU_COLOR_GAMMA_FILTER      1       // 曝光度
#define GPU_COLOR_SATURATION_FILTER 2       // 饱和度
#define GPU_COLOR_FADE_FILTER       3       // 褪色
#define GPU_COLOR_BLUR_FILTER       4       // 模糊
#define GPU_COLOR_SHARPNESS_FILTER  5       // 锐化
#define GPU_COLOR_TEMPERATURE_FILTER 6      // 色温
#define GPU_COLOR_TINT_FILTER       7       // 色调
#define GPU_COLOR_HIGHLIGHTS_FILTER 8       // 高光
#define GPU_COLOR_SHADOWS_FILTER    9       // 阴影
#define GPU_COLOR_VIGNETTE_FILTER   10      // 暗角

class GPUStreamFrame: public GPUGroupFilter{
public:
    static GPUStreamFrame*  shareInstance();
    static void destroyInstance();
    GPUStreamFrame();
    ~GPUStreamFrame();
    
    virtual void newFrame();
    
    // 美颜
    void setSmoothStrength(float strength);
    void setWhitenStrength(float strength);
    // lookup滤镜
    void setExtraFilter(const char* image);
    void setExtraFilter(const char* file, const char* image);
    //void setExtraFilter(const char* shader, const char* image);
    void removeExtraFilter();
    void setExtraParameter(float para);
    
    // 设置输入格式，支持rgba、nv21、nv12
    void setInputFormat(gpu_pixel_format_t format = GPU_RGBA);
    void setInputFilter(GPUFilter* input);
    // 输入旋转与尺寸
    void setInputRotation(gpu_rotation_t rotation);
    void setInputSize(uint32_t width, uint32_t height);
    // 预览
    void setPreviewRotation(gpu_rotation_t rotation);
    void setPreviewMirror(bool mirror);
    void setOutputView(GPUInput* view);
    void removeOutputView();
    // 预览logo
    void setPreviewBlend(GPUPicture* picture, gpu_rect_t rect, bool mirror);
    
    // 输出旋转与尺寸
    void setFrameRotation(gpu_rotation_t rotation);
    void setOutputRotation(gpu_rotation_t rotation);
    void setOutputMirror(bool mirror);
    virtual void setOutputSize(uint32_t width, uint32_t height);
    // 设置输出的byte格式，目前支持yuv420p, rgba, nv21, nv12
    void setOutputFormat(gpu_pixel_format_t format);
    void setVideoBlend(GPUPicture* picture, gpu_rect_t rect, bool mirror);

    void setStreamFrameSize(int width, int height);
    // 设置边框宽度与颜色
    void setBlank(int border, int r, int g, int b);
public:
    // GPUBeautyFilter     m_beauty_filter;
    //VSBeautyGroup       m_beauty_group;
    GPUGroupFilter      m_extra_group;
    GPUGroupFilter      m_output_group; // output不会再有target
    // 美颜
    GPUSmoothFilter     m_smooth_filter;
    //GPUWhiteningFilter  m_whiten_filter;
    // 滤镜
    GPUFilter*          m_extra_filter;
    GPUColorFilter      m_color_filter;
    GPUBlankFilter      m_blank_filter;
    // 预览
    GPUBlend2Filter     m_preview_blend_filter;
    // 输出
    GPUBlend2Filter     m_video_blend_filter;
    GPUZoomFilter       m_zoom_filter;

    GPURGBToYUVFilter*  m_yuv_filter;
    GPUToYUV420Filter*  m_yuv420_filter;
    GPUToNV21Filter*    m_nv21_filter;
    GPUToNV12Filter*    m_nv12_filter;
    GPURawOutput*       m_raw_output;
    
    GPUInput*           m_view;
    gpu_pixel_format_t  m_output_format;
    bool                m_outer_input;
    
    bool                m_beauty;     // 是否开启美颜
    
    uint32_t            m_frame_width;
    uint32_t            m_frame_height;
    int                 m_camera_position;
    
protected:
    static  GPUStreamFrame*  m_instance;
};

#endif

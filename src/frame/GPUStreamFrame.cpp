/**
 * file :	GPUStreamFrame.cpp
 * author :	Rex
 * create :	2018-07-10 21:09
 * func : 
 * history:
 */

#include "GPUStreamFrame.h"
#include "GPUFileFilter.h"

GPUStreamFrame* GPUStreamFrame::m_instance = NULL;

GPUStreamFrame* GPUStreamFrame::shareInstance(){
    if (m_instance==NULL) {
        m_instance = new GPUStreamFrame();
    }
    return m_instance;
}

void GPUStreamFrame::destroyInstance(){
    if (m_instance!=NULL)
    {
        delete m_instance;
        m_instance = NULL;
    }
}
GPUStreamFrame::GPUStreamFrame():
m_extra_group("ExtraFilterGroup"),
m_output_group("OutputGroup")
{
    m_instance = this;
    m_output_format = GPU_UNKNOWN;
    m_beauty = false;
    m_frame_width = 0;
    m_frame_height = 0;
    m_view = NULL;
    m_input = NULL;
    m_smooth_filter.m_filter_name = "smooth_filter";
    m_whiten_filter.m_filter_name = "whiten_filter";
    m_video_blend_filter.m_filter_name = "video_blend0";
    
    // m_input = &m_smooth_filter;
    setInputFormat(GPU_RGBA);
    m_smooth_filter.addTarget(&m_whiten_filter);
    m_whiten_filter.addTarget(&m_extra_group);
    m_extra_group.addTarget(&m_color_filter);
    m_color_filter.addTarget(&m_blank_filter);
    m_blank_filter.addTarget(&m_preview_blend_filter);
    m_blank_filter.addTarget(&m_video_blend_filter);
    m_video_blend_filter.addTarget(&m_output_group);

    m_video_blend_filter.disable();
    m_extra_group.disable();
    m_output = &m_output_group;
    
    // 输出
    m_zoom_filter.m_filter_name = "zoom_filter";
    m_yuv_filter = new GPURGBToYUVFilter();
    m_yuv420_filter = new GPUToYUV420Filter();
    m_nv21_filter = new GPUToNV21Filter();
    m_nv12_filter = new GPUToNV12Filter();
    m_raw_output = new GPURawOutput();
    m_output_group.setFirstFilter(&m_zoom_filter);
    m_output_group.setLastFilter(&m_zoom_filter);
    m_output_group.disable();
    
    m_extra_filter = NULL;
}

void GPUStreamFrame::newFrame(){
    // 先执行完所有异步任务，此处主要为切换滤镜
    GPUContext::shareInstance()->runAsyncTasks();
    GPUGroupFilter::newFrame();
}

#pragma --mark "输入格式"
void GPUStreamFrame::setInputFormat(gpu_pixel_format_t format){
    m_outer_input = false;
    switch (format) {
        case GPU_NV21:
        {
            m_input = new GPUNV21ToRGBFilter();
            info_log("nv21 input format");
            break;
        }
        case GPU_NV12:
        {
            m_input = new GPUNV12ToRGBFilter();
            info_log("nv12 input format");
            break;
        }
        case GPU_RGBA:
        default:
        {
            m_input = new GPUFilter();
            break;
        }
    }
    
    m_input->addTarget(&m_smooth_filter);
}

void GPUStreamFrame::setInputFilter(GPUFilter* input){
    if (input==NULL){
        return;
    }

    m_outer_input = true;
    input->addTarget(m_input);
    input->setOutputRotation(m_input->getOutputRotation());

    m_input = input;
}

#pragma --mark beauty
void GPUStreamFrame::setSmoothStrength(float strength){
    m_smooth_filter.setExtraParameter(strength);
}
void GPUStreamFrame::setWhitenStrength(float strength){
    m_whiten_filter.setStrength(strength);
}
#pragma --mark "Logo"
void GPUStreamFrame::setPreviewBlend(GPUPicture* picture, gpu_rect_t rect, bool mirror){
    m_preview_blend_filter.setBlendImage(picture, rect, mirror);
}
void GPUStreamFrame::setVideoBlend(GPUPicture* picture, gpu_rect_t rect, bool mirror){
    m_video_blend_filter.enable();
    m_video_blend_filter.setBlendImage(picture, rect, mirror);
}

#pragma --mark "滤镜"
void* changeExtraFilter(void* para){
    GPUStreamFrame* stream = GPUStreamFrame::shareInstance();

    if (stream->m_extra_filter!=NULL) {
        stream->m_extra_filter->removeAllTargets();
        delete stream->m_extra_filter;
    }

    stream->m_extra_filter = (GPUFilter*)para;
    stream->m_extra_group.setFirstFilter(stream->m_extra_filter);
    stream->m_extra_group.setLastFilter(stream->m_extra_filter);
    stream->m_extra_group.enable();

    return NULL;
}

void GPUStreamFrame::setExtraFilter(const char* image){
    GPULookupFilter* extra_filter = new GPULookupFilter();
    extra_filter->setLookupImage(image);
    if (extra_filter==NULL) {
        return;
    }
    
    GPUContext::shareInstance()->pushAsyncTask(changeExtraFilter, extra_filter);
}

void GPUStreamFrame::setExtraFilter(const char* file, const char* image){

    GPUFilter* extra_filter = new GPUFileFilter(file, image);
    if (extra_filter==NULL) {
        return;
    }
    
    GPUContext::shareInstance()->pushAsyncTask(changeExtraFilter, extra_filter);
    //info_log("Set Extra Filter %s", name);
}

void GPUStreamFrame::removeExtraFilter(){
    m_extra_group.disable();
    
    if (m_extra_filter!=NULL) {
        m_extra_filter->removeAllTargets();
        delete m_extra_filter;
        m_extra_filter = NULL;
    }
}

void GPUStreamFrame::setExtraParameter(float para){
    if (m_extra_filter!=NULL) {
        m_extra_filter->setExtraParameter(para);
    }
}

#pragma --mark "输出格式与预览"
void GPUStreamFrame::setOutputView(GPUInput* view){
    //    m_extra_group.removeAllTargets();
    //    m_extra_group.addTarget(view);
    //    m_extra_group.addTarget(&m_output_group);
    m_preview_blend_filter.removeAllTargets();
    m_preview_blend_filter.addTarget(view);
    m_view = view;
}

void GPUStreamFrame::removeOutputView(){
    m_preview_blend_filter.removeTarget(m_view);
    m_view = NULL;
}
void GPUStreamFrame::setOutputFormat(gpu_pixel_format_t format){
    if (format == GPU_UNKNOWN)
    {
        return;
    }
    
    m_output_format = format;
    m_zoom_filter.removeAllTargets();
    m_yuv_filter->removeAllTargets();
    m_output_group.enable();
    
    switch(m_output_format){
        case GPU_BGRA:
            m_zoom_filter.setOutputFormat(GPU_BGRA);
            err_log("output format bgra");
#ifdef __ANDROID__
            m_zoom_filter.addTarget(m_raw_output);
#endif
            break;
        case GPU_RGBA:
            m_zoom_filter.addTarget(m_raw_output);
            err_log("output format rgba");
            break;
        case GPU_I420:
            m_zoom_filter.addTarget(m_yuv_filter);
            m_yuv_filter->addTarget(m_yuv420_filter);
            m_yuv420_filter->addTarget(m_raw_output);
            err_log("output format yuv420p");
            break;
        case GPU_NV21:
            m_zoom_filter.addTarget(m_yuv_filter);
            m_yuv_filter->addTarget(m_nv21_filter);
            m_nv21_filter->addTarget(m_raw_output);
            err_log("output format nv21");
            break;
        case GPU_NV12:
            m_zoom_filter.addTarget(m_yuv_filter);
            m_yuv_filter->addTarget(m_nv12_filter);
            m_nv12_filter->addTarget(m_raw_output);
            err_log("output format nv12");
            break;
        default:
            err_log("Error: Not Support Format!");
            break;
    }
}

void GPUStreamFrame::setStreamFrameSize(int width, int height){
    m_blank_filter.setStreamFrameSize(width, height);
}

void GPUStreamFrame::setBlank(int border, int r, int g, int b){
    m_blank_filter.setBlank(border, r, g, b);
}

#pragma --mark "输入、输出的尺寸与旋转方向"
void GPUStreamFrame::setInputRotation(gpu_rotation_t rotation){
    err_log("set input rotation[%d]", rotation);
#if __ANDROID__
    // android旋转方向由g_texture_input控制
    GPUInput::setOutputRotation(rotation);
#else
    m_input->setOutputRotation(rotation);
#endif
}

void GPUStreamFrame::setInputSize(uint32_t width, uint32_t height){
    m_input->setOutputSize(width, height);
    m_frame_width = width;
    m_frame_height = height;
    info_log("input size[%u/%u]", width, height);
}
void GPUStreamFrame::setPreviewRotation(gpu_rotation_t rotation){
    if (m_view!=NULL) {
        m_view->setOutputRotation(rotation);
    }
}
void GPUStreamFrame::setOutputRotation(gpu_rotation_t rotation){
    info_log("set output rotation: %d", rotation);
    m_zoom_filter.setOutputRotation(rotation);
}
void GPUStreamFrame::setFrameRotation(gpu_rotation_t rotation){
    if(m_color_filter.m_frame_width==0 || m_color_filter.m_frame_width==0){
        err_log("setOutputRotation must be called after recv frame!!!");
        return;
    }

    info_log("set output rotation: %d", rotation);
    m_color_filter.m_shot_filter.setOutputRotation(rotation);
    // smooth和extra_group可能disable
    gpu_size_t size = m_whiten_filter.sizeOfFBO();
    if (rotation==GPURotateLeft||rotation==GPURotateRight||rotation==GPURotateRightFlipHorizontal||rotation==GPURotateRightFlipVertical){
        GPUContext::shareInstance()->glContextLock();
        m_color_filter.m_shot_filter.setOutputSize(size.height, size.width);
        GPUContext::shareInstance()->glContextUnlock();
    }
    else{
        GPUContext::shareInstance()->glContextLock();
        m_color_filter.m_shot_filter.setOutputSize(size.width, size.height);
        GPUContext::shareInstance()->glContextUnlock();
    }
}

void GPUStreamFrame::setOutputSize(uint32_t width, uint32_t height){
    GPUFilter::setOutputSize(width, height);
    m_zoom_filter.setOutputSize(width, height);
    info_log("output size[%u/%u]", width, height);
}

void GPUStreamFrame::setPreviewMirror(bool mirror){
    if (m_view!=NULL)
    {
#if __IOS__
        m_view->setOutputRotation(mirror ? GPUFlipHorizonal:GPUNoRotation);
#else
        m_view->setOutputRotation(mirror ? GPUFlipVertical:GPURotate180);
#endif
    }
}

void GPUStreamFrame::setOutputMirror(bool mirror){
#ifdef __ANDROID__
    m_zoom_filter.setOutputRotation(mirror ? GPUNoRotation:GPUFlipHorizonal);
#else
    m_zoom_filter.setOutputRotation(mirror ? GPUFlipHorizonal:GPUNoRotation);
#endif
    //    m_zoom_filter->setOutputRotation(mirror ? GPUFlipHorizonal:GPUNoRotation);
}

#pragma --mark "析构函数"
GPUStreamFrame::~GPUStreamFrame(){
    m_input->removeAllTargets();
    if (!m_outer_input)
    {
        delete m_input;
    }
    
    // m_beauty_group.removeAllTargets();
    // m_shaper_group.removeAllTargets();
    // m_background_group.removeAllTargets();
    // m_props_group.removeAllTargets();
    // m_extra_group.removeAllTargets();

    delete m_yuv_filter;
    delete m_yuv420_filter;
    delete m_raw_output;
    delete m_nv21_filter;
    delete m_nv12_filter;
    if (m_extra_filter!=NULL)
    {
        delete m_extra_filter;
    }
}

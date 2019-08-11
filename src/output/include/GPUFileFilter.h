/**
 * file :	GPUFileFilter.h
 * author :	Rex
 * create :	2016-12-13 20:46
 * func : 
 * history:
 */

#ifndef	__GPUFILEFILTER_H_
#define	__GPUFILEFILTER_H_

#include "GPUFilter.h"
#include "GPUPicture.h"

extern const char* g_file_filter_map[];
extern int g_file_filter_num;

class GPUFileFilter: public GPUFilter{
public:
    GPUFileFilter(const char* folder);
    ~GPUFileFilter();
    
    virtual void setInputFrameBuffer(GPUFrameBuffer* buffer, int location=0);
    
    bool exist(){ return m_exist; }
protected:
    //用于查表的图片
    std::vector<GPUPicture*>    m_images;
    bool    m_exist;
};

#endif

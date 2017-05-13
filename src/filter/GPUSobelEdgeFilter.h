/**
 * file :	GPUSobelEdgeFilter.h
 * author :	Rex
 * create :	2016-08-10 20:57
 * func : 
 * history:
 */

#ifndef	__GPUSOBELEDGEFILTER_H_
#define	__GPUSOBELEDGEFILTER_H_

#include "GPUTwoPassFilter.h"
#include "GPULuminanceFilter.h"

class GPUSobelEdgeFilter: public GPUTwoPassFilter{
public:
    GPUSobelEdgeFilter(float strength = 1.0);
    
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    virtual void setExtraParameter(float p);
    
protected:
    uint32_t 	m_frame_width;
    uint32_t	m_frame_height;
};

class GPUSobelDirectionFilter: public GPUFilter{
public:
    GPUSobelDirectionFilter(float strength = 1.0);
    virtual void setExtraParameter(float p);
};
#endif

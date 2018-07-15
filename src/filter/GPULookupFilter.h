/**
 * file :	GPULookupFilter.h
 * author :	Rex
 * create :	2018-07-14 22:59
 * func : 
 * history:
 */

#ifndef	__GPULOOKUPFILTER_H_
#define	__GPULOOKUPFILTER_H_

#include "GPUFilter.h"
#include "GPUPicture.h"

class GPULookupFilter: public GPUFilter{
public:
    GPULookupFilter();
    
    void setLookupImage(const char* path);
    void stopLookup();
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    
protected:
    GPUPicture*     m_lookup;
};
#endif

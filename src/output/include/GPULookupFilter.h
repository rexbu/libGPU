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
    GPULookupFilter(const char* path);
    ~GPULookupFilter();
    bool setLookupImage(const char* path);
    bool exist(){
        return m_lookup!=NULL;
    }
    
    virtual void setInputFrameBuffer(GPUFrameBuffer *buffer, int location = 0);
    virtual void setExtraParameter(float p);
    
protected:
    GPUPicture*     m_lookup;
};
#endif

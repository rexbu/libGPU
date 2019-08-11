/**
 * file :	GPUBlankFilter.h
 * author :	Rex
 * create :	2018-09-07 22:18
 * func : 
 * history:
 */

#ifndef	__GPUBLANKFILTER_H_
#define	__GPUBLANKFILTER_H_

#include "GPUFilter.h"

class GPUBlankFilter: public GPUFilter{
public:
    GPUBlankFilter(){
        m_border = 0;
    }
    void setStreamFrameSize(int width, int height);
    void setBlank(int border, int r, int g, int b);
    
protected:
    int         m_border;
};

#endif

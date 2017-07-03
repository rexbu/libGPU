/**
 * file :	GPUPicture.h
 * author :	Rex
 * email : rex@labjk.com
 * create :	2016-08-10 17:06
 * func : 
 * history:
 */

#ifndef	__GPUPICTURE_H_
#define	__GPUPICTURE_H_

#include "bs.h"
#include "GPUOutput.h"
#include "GPUFrameBuffer.h"

class GPUPicture: public GPUOutput{
public:
	GPUPicture(const char* path);
    GPUPicture(uint8_t * data, uint32_t size);
    GPUPicture(uint8_t * data, int width, int height);
#if __IOS__
	GPUPicture(void* cgImageRef);
    // pathForResource路径初始化
    GPUPicture(const char* file, const char* type);
#endif

    ~GPUPicture();
    
    virtual bool exist(){ return m_exist; }
    void processImage();
    void setPixel(uint8_t* pixels, uint32_t width, uint32_t height, uint32_t format=GL_RGBA);

    uint8_t*    m_pixels;
    gpu_size_t  m_image_size;
protected:
#if __IOS__
    void init(void* image);
#endif

    bool        hasProcessedImage;
    
    bool        m_exist;
};

#endif

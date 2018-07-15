/**
 * file :	GPUFileFilter.cpp
 * author :	Rex
 * create :	2016-12-13 20:46
 * func : 
 * history:
 */

#include "GPUFileFilter.h"
#include "GPUProgram.h"

GPUFileFilter::GPUFileFilter(const char* fragment, const char* image){
    m_filter_name = "FileFilter";

    char byte[10240];
    int fd = open(fragment, O_RDONLY);
    if (fd < 0){
        return;
    }

    long s = read(fd, byte, 10240);
    close(fd);

    GPUPicture* picture = new GPUPicture(image);
    m_images.push_back(picture);
    picture->addTarget(this, (int)m_images.size());

    // 初始化filter
    setInputs((int)(m_images.size()+1));

    m_program = new GPUProgram(g_vertex_shader, (const char*)byte, m_filter_name.c_str());
    init();
}
GPUFileFilter::GPUFileFilter(const char* fragment, const char* image[], int size){
    for (int i = 0; i < size; ++i) {
        GPUPicture* picture = new GPUPicture(image[i]);
        m_images.push_back(picture);
        picture->addTarget(this, (int)m_images.size());
    }
}

void GPUFileFilter::setInputFrameBuffer(GPUFrameBuffer* buffer, int location){
    GPUFilter::setInputFrameBuffer(buffer, location);
    // location=0时为filter链条传递，否则是image传递，image传递不再执行下面代码
    if (location==0) {
        for (int i=0; i<m_images.size(); i++) {
            m_images[i]->processImage();
        }
    }
}

GPUFileFilter::~GPUFileFilter(){
    for (int i=0; i<m_images.size(); i++) {
        delete m_images[i];
    }
    m_images.clear();
}

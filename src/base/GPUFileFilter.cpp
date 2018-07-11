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

const char* g_file_filter_map[] = {
    "origin",   "10000_3",
    "valencia", "10001_2",
    "vintage", "10003_2",
    "brannan",  "10004_2",
    "inkwell",  "10005_2",
    "pink",     "10014_4",
    "walden",  "10015_3",
    "grass",  "10020_5",
    "beach",  "10021_4",
    "sweety",  "10023_8",
    "nature",  "10024_8",
    "clean",  "10026_8",
    "fresh",  "10028_3",
    "coral",  "10029_3",
    "sunset",  "10030_5",
    "vivid",  "10031_2",
    "lolita",  "10032_3",
    "crisp",  "10033_2",
    "rosy",  "10034_2",
    "urban",  "10035_1",
    "vintage",  "10036_1",
    "smooth0",  "20000_1",
    "smooth1",  "20001_5",
    "smooth2",  "20002_5",
    "smooth3",  "20003_5",
    "smooth4",  "20004_5",
    "smooth5",  "20005_5",
    "no_face_eye",      "30001_2",
    "slimface",         "30002_2",
    "bigeye",           "30003_2",
    "slimfacebigeye",   "30004_2",
};

int g_file_filter_num = (sizeof(g_file_filter_map)/sizeof(const char*))/2;

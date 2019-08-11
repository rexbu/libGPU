/**
 * file :	GPUFileFilter.cpp
 * author :	Rex
 * create :	2016-12-13 20:46
 * func : 
 * history:
 */

#include "GPUFileFilter.h"
#include "GPUProgram.h"

GPUFileFilter::GPUFileFilter(const char* fdir){
    m_filter_name = "FileFilter";
    m_filter_name.append(fdir);
    m_exist = true;
    
    folder_t folder;
    folder_init(&folder);
    if(folder_open(&folder, fdir) != BS_SUCCESS){
        m_exist = false;
        folder_destroy(&folder);
        return;
    }
    
    // 读dat文件
    const char* data_path = folder_type(&folder, "dat");
    if (data_path==NULL) {
        m_exist = false;
        folder_destroy(&folder);
        return;
    }
    const char* data_buffer = folder_read(&folder, data_path);
    // 读idx文件
    const char* idx_path = folder_type(&folder, "idx");
    if (idx_path==NULL) {
        m_exist = false;
        folder_destroy(&folder);
        return;
    }
    FILE* idx_file = fopen(idx_path, "rb");
    char image[256];
    char start[16], length[16];
    while(fscanf(idx_file, "%[^:]:%[^:]:%[^;];", image, start, length)>0){
        //        char image_path[1024];
        //        sprintf(image_path, "%s/%s", path, image);
        GPUPicture* picture = new GPUPicture((uint8_t*)&data_buffer[atoi(start)], (uint32_t)atoi(length));
        m_images.push_back(picture);
        picture->addTarget(this, (int)m_images.size());
    }
    fclose(idx_file);
    
    // 初始化filter
    setInputs((int)(m_images.size()+1));
    const char* glsl = folder_read_file(&folder, "glsl");
    if (glsl==NULL) {
        m_exist = false;
        folder_destroy(&folder);
        return;
    }
    
    m_program = new GPUProgram(g_vertex_shader, glsl, m_filter_name.c_str());
    init();
    
    folder_destroy(&folder);
}
/*
GPUFileFilter::GPUFileFilter(const char* fragment, const char* image[], int size){
    for (int i = 0; i < size; ++i) {
        GPUPicture* picture = new GPUPicture(image[i]);
        m_images.push_back(picture);
        picture->addTarget(this, (int)m_images.size());
    }
}
*/
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

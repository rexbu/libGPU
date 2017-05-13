/**
 * file :	GPUVertexBuffer.h
 * author :	Rex
 * create :	2016-09-27 14:51
 * func : 
 * history:
 */

#ifndef	__GPUVERTEXBUFFER_H_
#define	__GPUVERTEXBUFFER_H_

#include <vector>
#include "McReference.h"
#include "GPUContext.h"

using namespace std;
using namespace mc;
class GPUVertexBuffer: public Reference{
public:
    GPUVertexBuffer(uint32_t vertexCount = 4, uint32_t dimension = 2, bool cache = false);
    ~GPUVertexBuffer();
    
    GLuint  getVertexBuffer(){
        return m_vertexbuffer;
    }
    uint32_t getDimension(){
        return m_dimension;
    }
    uint32_t getVertexCount(){
        return m_vertexcount;
    }
    uint32_t getDataSize(){
        return m_size;
    }
    bool isCache(){
        return m_cache;
    }
    
    void activeBuffer(GLuint attribute, const GLvoid *data, int offset=0);
    void disableBuffer(GLuint attribute);
protected:
    uint32_t        m_vertexcount;
    uint32_t        m_dimension;
    uint32_t        m_size;
    GLuint          m_vertexbuffer;
    
    bool            m_cache;    // 是否使用顶点缓存
};

class GPUVertexBufferCache{
public:
    static GPUVertexBufferCache* shareInstance();
    static void destroyInstance();
    // static GPUVertexBuffer* defaultVertexBuffer();
    // static GPUVertexBuffer* defaultCoordinateBuffer();
    
    GPUVertexBufferCache();
    ~GPUVertexBufferCache();
    
    GPUVertexBuffer* getVertexBuffer(uint32_t vertexCount = 4 , uint32_t dimension = 2, bool cache = false);
    virtual GPUVertexBuffer* newVertexBuffer(uint32_t vertexCount = 4 , uint32_t dimension = 2, bool cache = false);
    void clear();
    /**
     * 设置是否使用顶点缓存。
     */
    static void setCache(bool cache){
        m_cache = cache;
    }
    
public:
    static GPUVertexBufferCache*    m_instance;
    static bool                     m_cache;
    
    vector<GPUVertexBuffer*>    m_bufferlist;
};
#endif

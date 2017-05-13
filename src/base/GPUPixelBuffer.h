/**
 * file :	GPUPixelBuffer.h
 * author :	Rex
 * create :	2016-07-21 16:58
 * func : 
 * history:
 */

#ifndef	__GPUPIXELBUFFER_H_
#define	__GPUPIXELBUFFER_H_

#include "GPUContext.h"

enum{
	GPU_UNPACK,		// CPU向gpu拷贝
	GPU_PACK,		// gpu向cpu拷贝
};

class GPUPixelBuffer{
public:
	GPUPixelBuffer();
	GPUPixelBuffer(GLuint texture, uint32_t width=0, uint32_t height=0, int pack = GPU_PACK);
	~GPUPixelBuffer();
	// 向texture写入数据
	void upload(GLubyte* byte, uint32_t format = GL_RGBA);
	void upload(GLuint texture, GLubyte* byte, uint32_t width, uint32_t height, uint32_t format = GL_RGBA);
	// 从texture读取数据
	GLubyte* download();
	GLubyte* download(GLuint texture, uint32_t width, uint32_t height);

protected:
	void unpack();
	void pack();

	GLuint		m_texture;
	GLuint		m_buffer;
	GLubyte*	m_ptr;

	uint32_t	m_width;
	uint32_t	m_height;

	int 		m_pack_mode;
};
#endif

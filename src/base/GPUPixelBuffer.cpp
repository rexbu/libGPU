/**
 * file :	GPUPixelBuffer.cpp
 * author :	Rex
 * create :	2016-07-21 16:58
 * func : 
 * history:
 */

#include "GPUPixelBuffer.h"

GPUPixelBuffer::GPUPixelBuffer(){
	m_texture = 0;
	m_width = 0;
	m_height = 0;
	m_pack_mode = GPU_PACK;
	glGenBuffers(1, &m_buffer);
}

GPUPixelBuffer::GPUPixelBuffer(GLuint texture, uint32_t width, uint32_t height, int isPack){
	m_texture = texture;
	m_width = width;
	m_height = height;
	m_pack_mode = isPack;
	glGenBuffers(1, &m_buffer);

	if (isPack==GPU_PACK)
	{
		pack();
	}
	else{
		unpack();
	}
}

void GPUPixelBuffer::upload(GLubyte* bytes, uint32_t format){
	unpack();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, 0);
	switch(format){
	case GL_LUMINANCE:
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height, bytes, GL_STREAM_DRAW);
		break;
	case GL_LUMINANCE_ALPHA:
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height*2, bytes, GL_STREAM_DRAW);
		break;	
	default:
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height*4, bytes, GL_STREAM_DRAW);
	}
}

void GPUPixelBuffer::unpack(){
	GPUContext::shareInstance()->makeCurrent();
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_buffer);
}

GLubyte* GPUPixelBuffer::download(){
	return m_ptr;
}

GLubyte* GPUPixelBuffer::download(GLuint texture, uint32_t width, uint32_t height){
	m_texture = texture;
	m_width = width;
	m_height = height;
	m_pack_mode = GPU_PACK;
	pack();
	return m_ptr;
}

void GPUPixelBuffer::pack(){
	GPUContext::shareInstance()->makeCurrent();
	glReadBuffer(GL_FRONT);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer);
	glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	// m_ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_width*m_height*4, GL_READ_WRITE);
}

GPUPixelBuffer::~GPUPixelBuffer(){
	if (m_pack_mode==GPU_PACK)
	{
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}
	else{
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);	
	}
	glDeleteBuffers(1, &m_buffer);
}

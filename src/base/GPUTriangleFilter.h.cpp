/**
 * file :	GPUVertexFilter.cpp
 * author :	Rex
 * create :	2017-02-25 23:32
 * func : 
 * history:
 */

#include "GPUTriangleFilter.h"

GPUTriangleFilter::GPUTriangleFilter(const char* fragment, int inputs, int vertex):
GPUFilter(fragment, inputs){
    m_vertices.resize(vertex*2);
    m_input_coordinates.resize(vertex*2);
}

void GPUTriangleFilter::setVertex(float* vertex, int count){
    m_vertices.resize(count*2);
    m_input_coordinates.resize(count*2);
}

void GPUTriangleFilter::render(){
    
}

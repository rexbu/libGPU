/**
 * file :	GPUGaussianBlurFilter.cpp
 * author :	Rex
 * create :	2016-07-13 23:29
 * func : 
 * history:
 */

#include <math.h>
#include "GPUGaussianBlurFilter.h"

const static char* g_gaussian_fragment_shader = SHADER_STRING(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture[1];
    uniform lowp vec2 unblur_center;
    uniform lowp vec2 unblur_raduis;
    uniform mediump float xStep;
    uniform mediump float yStep;
    void main()
    {
        // 长宽不一需要计算椭圆
        if(unblur_raduis.x>0.0 && unblur_raduis.y>0.0){
            mediump float dis_x = pow((textureCoordinate.x-unblur_center.x)/unblur_raduis.x,2.0);
            mediump float dis_y = pow((textureCoordinate.y-unblur_center.y)/unblur_raduis.y,2.0);
            if(dis_x+dis_y < 1.0){
                gl_FragColor = texture2D(inputImageTexture[0], textureCoordinate);
                return;
            }
        }
        
        mediump vec4 value = vec4(0.0);
        mediump vec2 stepOffset = vec2(xStep, yStep);
        
        %s
        gl_FragColor = value;
    }
);

GPUGaussianBlurFilter::GPUGaussianBlurFilter(uint32_t radius, float sigma):m_x_filter(generateShader(radius, sigma)), m_y_filter(generateShader(radius, sigma)){
    m_input = &m_x_filter;
    m_output = &m_y_filter;
    m_x_filter.addTarget(&m_y_filter);
    
    m_unblur_x = 0;
    m_unblur_y = 0;
    m_unblur_radius = 0;
}

void GPUGaussianBlurFilter::setExtraParameter(float sigma){
    float blurRadiusInPixels = round(sigma); // For now, only do integral sigmas
    
    uint32_t calculatedSampleRadius = 0;
    if (blurRadiusInPixels >= 1) // Avoid a divide-by-zero error here
    {
        // Calculate the number of pixels to sample from by setting a bottom limit for the contribution of the outermost pixel
        float minimumWeightToFindEdgeOfSamplingArea = 1.0/256.0;
        calculatedSampleRadius = floor(sqrt(-2.0 * pow((double)blurRadiusInPixels, 2.0) * log(minimumWeightToFindEdgeOfSamplingArea * sqrt(2.0 * M_PI * pow((double)blurRadiusInPixels, 2.0))) ));
        calculatedSampleRadius += calculatedSampleRadius % 2; // There's nothing to gain from handling odd radius sizes, due to the optimizations I use
    }

    m_x_filter.changeShader(generateShader(calculatedSampleRadius, blurRadiusInPixels));
    m_y_filter.changeShader(generateShader(calculatedSampleRadius, blurRadiusInPixels));

    m_x_filter.setFloat("xStep", 1.0/(m_frame_width-1));
    m_x_filter.setFloat("yStep", 0.0);
    m_y_filter.setFloat("xStep", 0.0);
    m_y_filter.setFloat("yStep", 1.0/(m_frame_height-1));
    setUnBlurRegion();
}

char* GPUGaussianBlurFilter::generateShader(uint32_t radius, float sigma){
    m_pixel_radius = radius;
    m_pixel_sigma = sigma;

    char circalValue[10240] = {0};
    float* weight = (float*)malloc(sizeof(float)*(radius+1));
    float sumWeight = 0;
    
    const char* value_p = "value += %f * texture2D(inputImageTexture[0], textureCoordinate + stepOffset*%f);";
    const char* value_d = "value += %f * texture2D(inputImageTexture[0], textureCoordinate - stepOffset*%f);";
    for (int i=0; i<radius+1; i++) {
        weight[i] = (1.0 / sqrt(2.0 * M_PI * pow((double)sigma, 2.0))) * exp(-pow((double)i, 2.0) / (2.0 * pow((double)sigma, 2.0)));
        
        if (i == 0)
        {
            sumWeight += weight[i];
        }
        else
        {
            sumWeight += 2.0 * weight[i];
        }
    }
    for (int i=0; i<radius+1; i++) {
        weight[i] = weight[i]/sumWeight;
    }

    int pointNum = bs_min(radius / 2 + (radius % 2), 7);
    int largeOptimizedOffsets = radius / 2 + (radius % 2);
    
    snprintf(circalValue+strlen(circalValue), sizeof(circalValue)-strlen(circalValue), "value += %f * texture2D(inputImageTexture[0], textureCoordinate);",
             weight[0]);
//    for (int i=1; i<radius+1; i++) {
//        snprintf(circalValue+strlen(circalValue), sizeof(circalValue)-strlen(circalValue), value_p,
//                 weight[i], i*1.0);
//        snprintf(circalValue+strlen(circalValue), sizeof(circalValue)-strlen(circalValue), value_d,
//                 weight[i], i*1.0);
//    }
    for (int i = 0; i < pointNum; i++)
    {
        float firstWeight = weight[i*2 + 1];
        float secondWeight = weight[i*2 + 2];
        float optimizedWeight = firstWeight + secondWeight;
        float optimizedOffset = (firstWeight * (i*2 + 1) + secondWeight * (i*2 + 2)) / optimizedWeight;

        snprintf(circalValue+strlen(circalValue), sizeof(circalValue)-strlen(circalValue), value_p,
                 optimizedWeight, optimizedOffset);
        snprintf(circalValue+strlen(circalValue), sizeof(circalValue)-strlen(circalValue), value_d,
                 optimizedWeight, optimizedOffset);
    }
    
    if (largeOptimizedOffsets>pointNum) {
//        snprintf(circalValue+strlen(circalValue), sizeof(circalValue)-strlen(circalValue),
//                 "highp vec2 singleStepOffset = vec2(xStep, yStep);\n");
        
        for (int i = pointNum; i < largeOptimizedOffsets; i++)
        {
            GLfloat firstWeight = weight[i * 2 + 1];
            GLfloat secondWeight = weight[i * 2 + 2];
            
            GLfloat optimizedWeight = firstWeight + secondWeight;
            GLfloat optimizedOffset = (firstWeight * (i * 2 + 1) + secondWeight * (i * 2 + 2)) / optimizedWeight;
            
            snprintf(circalValue+strlen(circalValue), sizeof(circalValue)-strlen(circalValue),
                     "value += texture2D(inputImageTexture[0], textureCoordinate+stepOffset*%f) * %f;\n", optimizedOffset, optimizedWeight);
            snprintf(circalValue+strlen(circalValue), sizeof(circalValue)-strlen(circalValue),
                     "value += texture2D(inputImageTexture[0], textureCoordinate-stepOffset*%f) * %f;\n", optimizedOffset, optimizedWeight);
        }
    }

	snprintf(m_fragment, sizeof(m_fragment), g_gaussian_fragment_shader,
		circalValue);
    free(weight);
    
    return m_fragment;
}

void GPUGaussianBlurFilter::setInputFrameBuffer(GPUFrameBuffer *buffer, int location){
    // 如果尺寸发生变化，重新设置尺寸
    if (m_frame_width!=buffer->m_width || m_frame_height!=buffer->m_height)
    {
        m_frame_width = buffer->m_width;
        m_frame_height = buffer->m_height;
        m_x_filter.setFloat("xStep", 1.0/(m_frame_width-1));
        m_x_filter.setFloat("yStep", 0.0);
        m_y_filter.setFloat("xStep", 0.0);
        m_y_filter.setFloat("yStep", 1.0/(m_frame_height-1));
        setUnBlurRegion();
    }
    GPUGroupFilter::setInputFrameBuffer(buffer, location);
}

void GPUGaussianBlurFilter::setUnBlurRegion(int x, int y, int radius){
    m_unblur_x = x;
    m_unblur_y = y;
    m_unblur_radius = radius;
    if (m_frame_width>0 && m_frame_height>0) {
        setUnBlurRegion();
    }
}

void GPUGaussianBlurFilter::setUnBlurRegion(){
    float center[2] = {m_unblur_x*1.0f/m_frame_width, m_unblur_y*1.0f/m_frame_height};
    float radius[2] = {m_unblur_radius*1.0f/m_frame_width, m_unblur_radius*1.0f/m_frame_height};
    m_x_filter.setFloat("unblur_center", center, 2);
    m_x_filter.setFloat("unblur_raduis", radius, 2);
    m_y_filter.setFloat("unblur_center", center, 2);
    m_y_filter.setFloat("unblur_raduis", radius, 2);
}

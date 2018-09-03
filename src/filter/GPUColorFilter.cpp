/**
 * file :	GPUContrastFilter.cpp
 * author :	Rex
 * create :	2017-05-26 12:13
 * func : 
 * history:
 */

#include "GPUColorFilter.h"

#pragma --mark "shader"

const static char* g_contrast_fragemtn_shader = SHADER_STRING(
varying highp vec2 textureCoordinate;

uniform sampler2D inputImageTexture[1];

// 对比度 -1~1
uniform lowp float contrast;
// 曝光 -1~1
uniform lowp float gamma;
// 饱和 -1~1
uniform lowp float saturation;
// 褪色 0~1
uniform lowp float fadeIntensity;

uniform lowp int spareMargin;
uniform mediump vec2 lowerLeft;
uniform mediump vec2 upperRight;

// Values from "Graphics Shaders: Theory and Practice" by Bailey and Cunningham"
const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

void main() {
    lowp vec4 textureColor = texture2D(inputImageTexture[0], textureCoordinate);
    lowp vec4 result = textureColor;

    if (spareMargin == 1 &&
      (textureCoordinate.x < lowerLeft.x ||
       textureCoordinate.y > lowerLeft.y ||
       textureCoordinate.x > upperRight.r ||
       textureCoordinate.y < upperRight.y)) {
          gl_FragColor = result;
          return;
    }

    // gamma
    result = vec4(pow(result.rgb, vec3(gamma)), result.w);

    // contrast
    result = vec4(((result.rgb - vec3(0.5)) * contrast + vec3(0.5)), result.w);

    // saturation
    lowp float luminance = dot(result.rgb, luminanceWeighting);
    lowp vec3 greyScaleColor = vec3(luminance);

    result = vec4(mix(greyScaleColor, result.rgb, saturation), result.w);

    // fade
    highp float inRed = result.r * 255.0;
    highp float inGreen = result.g * 255.0;
    highp float inBlue = result.b * 255.0;
    highp float outRed = 63.0 + inRed * (-0.176624 + inRed * (0.00777 + inRed * (-0.00001804)));
    highp float outGreen = 63.0 + inGreen * (-0.176624 + inGreen * (0.00777 + inGreen * (-0.00001804)));
    highp float outBlue = 63.0 + inBlue * (-0.176624 + inBlue * (0.00777 + inBlue * (-0.00001804)));
    lowp vec3 outColor = vec3(outRed / 255.0, outGreen / 255.0, outBlue / 255.0);

    result = mix(result, vec4(outColor, result.a), fadeIntensity);

    gl_FragColor = result;
}
);


static const char* g_adjust_vertex_shader = SHADER_STRING(
attribute vec4 position;
attribute vec4 inputTextureCoordinate;

uniform float imageWidthFactor;
uniform float imageHeightFactor;
// 锐化
uniform float sharpness;

varying vec2 textureCoordinate;
varying vec2 leftTextureCoordinate;
varying vec2 rightTextureCoordinate;
varying vec2 topTextureCoordinate;
varying vec2 bottomTextureCoordinate;

varying float centerMultiplier;
varying float edgeMultiplier;

void main()
{
    gl_Position = position;
    
    textureCoordinate = inputTextureCoordinate.xy;
    
    mediump vec2 widthStep = vec2(imageWidthFactor, 0.0);
    mediump vec2 heightStep = vec2(0.0, imageHeightFactor);
    
    leftTextureCoordinate = inputTextureCoordinate.xy - widthStep;
    rightTextureCoordinate = inputTextureCoordinate.xy + widthStep;
    topTextureCoordinate = inputTextureCoordinate.xy + heightStep;
    bottomTextureCoordinate = inputTextureCoordinate.xy - heightStep;
    
    centerMultiplier = 1.0 + 4.0 * sharpness;
    edgeMultiplier = sharpness;
}
);

static const char* g_adjust_fragment_shader = SHADER_STRING(
const lowp vec3 warmFilter = vec3(0.93, 0.54, 0.0);
uniform lowp vec2 vignetteCenter;
const lowp vec3 vignetteColor = vec3(0, 0, 0);
const lowp float vignetteStart = 0.35;

const mediump mat3 RGBtoYIQ = mat3(0.299, 0.587, 0.114, 0.596, -0.274, -0.322, 0.212, -0.523, 0.311);
const mediump mat3 YIQtoRGB = mat3(1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.105, 1.702);
const mediump vec3 luminanceWeighting = vec3(0.3, 0.3, 0.3);

varying highp vec2 textureCoordinate;
varying highp vec2 leftTextureCoordinate;
varying highp vec2 rightTextureCoordinate;
varying highp vec2 topTextureCoordinate;
varying highp vec2 bottomTextureCoordinate;

varying highp float centerMultiplier;
varying highp float edgeMultiplier;

uniform sampler2D inputImageTexture[1];
// 色温
uniform lowp float temperature;
// 色调
uniform lowp float tint;
// 阴影
uniform lowp float shadows;
// 高光
uniform lowp float highlights;
// 模糊
uniform lowp float vignetteIntensity;

uniform lowp int spareMargin;
uniform mediump vec2 lowerLeft;
uniform mediump vec2 upperRight;

void main()
{
    mediump vec3 centralColor = texture2D(inputImageTexture[0], textureCoordinate).rgb;
    
    if (spareMargin == 1 &&
        (textureCoordinate.x < lowerLeft.x ||
         textureCoordinate.y < (1.0 - lowerLeft.y) ||
         textureCoordinate.x > upperRight.r ||
         textureCoordinate.y > (1.0 - upperRight.y))) {
            gl_FragColor = vec4(centralColor, 1.0);
            return;
        }
    
    mediump vec3 leftColor = texture2D(inputImageTexture[0], leftTextureCoordinate).rgb;
    mediump vec3 rightColor = texture2D(inputImageTexture[0], rightTextureCoordinate).rgb;
    mediump vec3 topColor = texture2D(inputImageTexture[0], topTextureCoordinate).rgb;
    mediump vec3 bottomColor = texture2D(inputImageTexture[0], bottomTextureCoordinate).rgb;
    
    // sharpen
    vec4 result = vec4(
                       (centralColor * centerMultiplier -
                        (leftColor * edgeMultiplier +
                         rightColor * edgeMultiplier +
                         topColor * edgeMultiplier +
                         bottomColor * edgeMultiplier)),
                       texture2D(inputImageTexture[0], bottomTextureCoordinate).w);
    
    // white balance
    mediump vec3 yiq = RGBtoYIQ * result.rgb;
    yiq.b = clamp(yiq.b + tint*0.5226*0.1, -0.5226, 0.5226);
    lowp vec3 rgb = YIQtoRGB * yiq;
    
    lowp vec3 processed = vec3(
                               (rgb.r < 0.5 ? (2.0 * rgb.r * warmFilter.r) : (1.0 - 2.0 * (1.0 - rgb.r) * (1.0 - warmFilter.r))), //adjusting temperature
                               (rgb.g < 0.5 ? (2.0 * rgb.g * warmFilter.g) : (1.0 - 2.0 * (1.0 - rgb.g) * (1.0 - warmFilter.g))),
                               (rgb.b < 0.5 ? (2.0 * rgb.b * warmFilter.b) : (1.0 - 2.0 * (1.0 - rgb.b) * (1.0 - warmFilter.b))));
    result = vec4(mix(rgb, processed, temperature), result.a);
    
    // highlights
    mediump float lum1 = dot(result.rgb, luminanceWeighting);
    mediump float shadow1 = clamp((lum1 + (-0.76)*pow(lum1, 2.0)) - lum1, 0.0, 1.0);
    mediump float highlight1 = clamp(
                                     (1.0 - (pow(1.0-lum1, 1.0/(2.0-highlights)) + (-0.8)*pow(1.0-lum1, 2.0/(2.0-highlights)))) - lum1, -1.0, 0.0);
    
    lowp vec3 mixed1 = (lum1 + shadow1 + highlight1 ) * (result.rgb / lum1);
    
    result = vec4(mixed1.rgb, result.a);
    
    // shadows
    mediump float lum2 = dot(result.rgb, luminanceWeighting);
    mediump float shadow2 = clamp(
                                  (pow(lum2, 1.0/(shadows+1.0)) + (-0.76)*pow(lum2, 2.0/(shadows+1.0))) - lum2, 0.0, 1.0);
    mediump float highlight2 = clamp((1.0 - (1.0-lum2 + (-0.8)*pow(1.0-lum2, 2.0))) - lum2, -1.0, 0.0);
    
    lowp vec3 mixed2 = (lum2 + shadow2 + highlight2) * (result.rgb / lum2);
    
    result = vec4(mixed2.rgb, result.a);
    
    // vignette
    if (vignetteIntensity > 0.0) {
        lowp float vignetteEnd = 1.5 - 0.45 * vignetteIntensity; //same as 1.5 * (0.7 + 0.3 * (1 - vignetteIntensity));
        lowp float d = 0.0;
        if (spareMargin == 1) {
            lowp float dx = abs(textureCoordinate.x - vignetteCenter.x) / abs(lowerLeft.x - upperRight.x);
            lowp float dy = abs(textureCoordinate.y - 1.0 + vignetteCenter.y) / abs(lowerLeft.y - upperRight.y);
            d = sqrt(dx * dx + dy * dy);
        } else {
            d = distance(textureCoordinate, vignetteCenter);
        }
        
        lowp float percent = smoothstep(vignetteStart, vignetteEnd, d);
        result = vec4(mix(result.rgb, vignetteColor, percent), result.a);
    }
    
    gl_FragColor = result;
}
);

GPUColorFilter::GPUColorFilter():
m_shot_filter(g_contrast_fragemtn_shader),
m_adjust_filter(g_adjust_vertex_shader, g_adjust_fragment_shader){
    m_input = &m_shot_filter;
    m_output = &m_shot_filter;
    //m_shot_filter.addTarget(&m_adjust_filter);
    setContrast(0);
    setGamma(0);
    setSaturation(0);
    setFade(0);
}

#pragma --mark "对比、曝光、饱和、褪色"
// 对比度
void GPUColorFilter::setContrast(float p){
    m_shot_filter.setFloat("contrast", p/2+1);
}
// 曝光度
void GPUColorFilter::setGamma(float p){
    m_shot_filter.setFloat("gamma", (p/-2+1));
}
// 饱和度
void GPUColorFilter::setSaturation(float p){
    m_shot_filter.setFloat("saturation", p/2+1);
}
// 褪色
void GPUColorFilter::setFade(float p){
    m_shot_filter.setFloat("fadeIntensity", p/1.5);
}

#pragma --mark "模糊、锐化、色温、色调、高光、阴影、暗角"
// 模糊 [0,1]
void GPUColorFilter::setVignette(float x, float y, float intensity){
    float center[2] = {x, y};
    m_adjust_filter.setFloat("vignetteCenter", center, 2);
    m_adjust_filter.setFloat("vignetteIntensity", intensity);
}
// 锐化 [0,1]
void GPUColorFilter::setSharpness(float p){
    m_adjust_filter.setFloat("sharpness", p);
}
// 色温 [-1,1]
void GPUColorFilter::setTemperature(float p){
    m_adjust_filter.setFloat("temperature", p);
}
// 色调 [-1,1]
void GPUColorFilter::setTint(float p){
    m_adjust_filter.setFloat("tint", p);
}
// 高光 [0,1]
void GPUColorFilter::setHighlights(float p){
    m_adjust_filter.setFloat("highlights", p);
}
// 阴影 [0,1]
void GPUColorFilter::setShadows(float p){
    m_adjust_filter.setFloat("shadows", p);
}

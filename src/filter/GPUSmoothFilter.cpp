/**
 * file :	GPUSmoothFilter.cpp
 * author :	Rex
 * create :	2016-12-06 23:16
 * func : 
 * history:
 */

#include "GPUSmoothFilter.h"

const static char* g_smooth_filter = SHADER_STRING(
	#define x_a 640.0
	#define y_a 1136.0
	uniform sampler2D inputImageTexture;
	varying lowp vec2 textureCoordinate;
	uniform mediump float strength;

	void main(){

	vec3 centralColor;

	float mul_x = 2.0 / x_a;
	float mul_y = 2.0 / y_a;

	vec2 blurCoordinates0 = textureCoordinate + vec2(0.0 * mul_x,-10.0 * mul_y);
	vec2 blurCoordinates1 = textureCoordinate + vec2(5.0 * mul_x,-8.0 * mul_y);
	vec2 blurCoordinates2 = textureCoordinate + vec2(8.0 * mul_x,-5.0 * mul_y);
	vec2 blurCoordinates3 = textureCoordinate + vec2(10.0 * mul_x,0.0 * mul_y);
	vec2 blurCoordinates4 = textureCoordinate + vec2(8.0 * mul_x,5.0 * mul_y);
	vec2 blurCoordinates5 = textureCoordinate + vec2(5.0 * mul_x,8.0 * mul_y);
	vec2 blurCoordinates6 = textureCoordinate + vec2(0.0 * mul_x,10.0 * mul_y);
	vec2 blurCoordinates7 = textureCoordinate + vec2(-5.0 * mul_x,8.0 * mul_y);
	vec2 blurCoordinates8 = textureCoordinate + vec2(-8.0 * mul_x,5.0 * mul_y);
	vec2 blurCoordinates9 = textureCoordinate + vec2(-10.0 * mul_x,0.0 * mul_y);
	vec2 blurCoordinates10 = textureCoordinate + vec2(-8.0 * mul_x,-5.0 * mul_y);
	vec2 blurCoordinates11 = textureCoordinate + vec2(-5.0 * mul_x,-8.0 * mul_y);

	mul_x = 1.6 / x_a;
	mul_y = 1.6 / y_a;

	vec2 blurCoordinates12 = textureCoordinate + vec2(0.0 * mul_x,-6.0 * mul_y);
	vec2 blurCoordinates13 = textureCoordinate + vec2(-4.0 * mul_x,-4.0 * mul_y);
	vec2 blurCoordinates14 = textureCoordinate + vec2(-6.0 * mul_x,0.0 * mul_y);
	vec2 blurCoordinates15 = textureCoordinate + vec2(-4.0 * mul_x,4.0 * mul_y);
	vec2 blurCoordinates16 = textureCoordinate + vec2(0.0 * mul_x,6.0 * mul_y);
	vec2 blurCoordinates17 = textureCoordinate + vec2(4.0 * mul_x,4.0 * mul_y);
	vec2 blurCoordinates18 = textureCoordinate + vec2(6.0 * mul_x,0.0 * mul_y);
	vec2 blurCoordinates19 = textureCoordinate + vec2(4.0 * mul_x,-4.0 * mul_y);


	float central;
	float gaussianWeightTotal;
	float sum;
	float sample;
	float distanceFromCentralColor;
	float gaussianWeight;

	float distanceNormalizationFactor = 3.6;

	central = texture2D(inputImageTexture, textureCoordinate).g;
	gaussianWeightTotal = 0.2;
	sum = central * 0.2;

	sample = texture2D(inputImageTexture, blurCoordinates0).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates1).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates2).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates3).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates4).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates5).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates6).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates7).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates8).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates9).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates10).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates11).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.08 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;



	sample = texture2D(inputImageTexture, blurCoordinates12).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.1 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates13).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.1 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates14).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.1 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates15).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.1 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates16).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.1 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates17).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.1 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates18).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.1 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sample = texture2D(inputImageTexture, blurCoordinates19).g;
	distanceFromCentralColor = min(abs(central - sample) * distanceNormalizationFactor, 1.0);
	gaussianWeight = 0.1 * (1.0 - distanceFromCentralColor);
	gaussianWeightTotal += gaussianWeight;
	sum += sample * gaussianWeight;

	sum = sum/gaussianWeightTotal;



	centralColor = texture2D(inputImageTexture, textureCoordinate).rgb;

	sample = centralColor.g - sum + 0.5;



	for(int i = 0; i < 5; ++i)
	{
	if(sample <= 0.5)
	{
	sample = sample * sample * 2.0;
	}
	else
	{
	sample = 1.0 - ((1.0 - sample)*(1.0 - sample) * 2.0);
	}
	}


	float aa = 1.0 + pow(sum, 0.3)*0.09;
	vec3 smoothColor = centralColor*aa - vec3(sample)*(aa-1.0);// get smooth color
	smoothColor = clamp(smoothColor,vec3(0.0),vec3(1.0));//make smooth color right

	smoothColor = mix(centralColor, smoothColor, pow(centralColor.g, 0.33));
	smoothColor = mix(centralColor, smoothColor, pow(centralColor.g, 0.39));

	smoothColor = mix(centralColor, smoothColor, strength);

	gl_FragColor = vec4(pow(smoothColor, vec3(0.96)),1.0);
	}
);

GPUSmoothFilter::GPUSmoothFilter():GPUFilter(g_smooth_filter, 1, "SmoothFilter"){
	setExtraParameter(0.0f);
}

void GPUSmoothFilter::setExtraParameter(float p){
	m_strength = p;
	setFloat("strength", p);
	 if (m_strength>0.0f && m_disable)
	 {
	 	enable();
	 }
	 else if (m_strength<=0.0f && !m_disable)
	 {
	 	disable();
	 }
}

void GPUSmoothFilter::setSmoothLevel(int level){
	float strength[6] = {0.0, 0.54, 0.69, 0.79, 0.90, 0.99};
	if (level<0)
	{
		level = 0;
	}

	if (level>5)
	{
		level = 5;
	}

	setExtraParameter(strength[level]);
}

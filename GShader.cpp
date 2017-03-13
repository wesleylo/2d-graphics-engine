/*
 *  Copyright 2015 Wesley Lo
 */

#include "GShader.h"
#include "MyShaderFromBitmap.h"
#include "MyShaderFromLinearGradient.h"
#include "MyShaderFromRadialGradient.h"

GShader* GShader::FromBitmap(const GBitmap& bitmap, const float localMatrix[6]) {
	return new MyShaderFromBitmap(bitmap, localMatrix);
}

GShader* GShader::FromLinearGradient(const GPoint pts[2], const GColor colors[2]) {
	return new MyShaderFromLinearGradient(pts, colors);
}

GShader* GShader::FromRadialGradient(const GPoint& center, float radius, const GColor colors[2]) {
	return new MyShaderFromRadialGradient(center, radius, colors);
}

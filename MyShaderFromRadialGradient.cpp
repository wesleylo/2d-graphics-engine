/*
 *  Copyright 2015 Wesley Lo
 */

#include "MyShaderFromRadialGradient.h"

MyShaderFromRadialGradient::MyShaderFromRadialGradient(const GPoint& center, float radius, const GColor colors[2]) {
	this->center = center;
	this->radius = radius;
	this->colors[0] = GColor::MakeARGB(colors[0].fA, colors[0].fR, colors[0].fG, colors[0].fB);
	this->colors[1] = GColor::MakeARGB(colors[1].fA, colors[1].fR, colors[1].fG, colors[1].fB);
}

bool MyShaderFromRadialGradient::setContext(const float ctm[6]) {
	for (int i = 0; i < 6; ++i) {
		this->ctm[i] = ctm[i];
	}
	float xTransformed = ctm[0] * center.fX + ctm[1] * center.fY + ctm[2];
	float yTransformed = ctm[3] * center.fX + ctm[4] * center.fY + ctm[5];
	centerTransformed = GPoint::Make(xTransformed, yTransformed);
	radiusTransformed = ctm[0] * radius;
	return true;
}

void MyShaderFromRadialGradient::shadeRow(int dst_x, int dst_y, int count, GPixel dst_row[]) {
	float src_a0 = colors[0].fA * 255;
	float src_r0 = colors[0].fR * 255;
	float src_g0 = colors[0].fG * 255;
	float src_b0 = colors[0].fB * 255;

	float src_r1 = colors[1].fR * 255;
	float src_g1 = colors[1].fG * 255;
	float src_b1 = colors[1].fB * 255;

	float range_r = src_r1 - src_r0;
	float range_g = src_g1 - src_g0;
	float range_b = src_b1 - src_b0;

	for (int i = dst_x; i < dst_x + count; ++i) {
		float x = (i - centerTransformed.fX);
		float y = (dst_y - centerTransformed.fY);
		float z_sq = x * x + y * y;
		float r_sq = radiusTransformed * radiusTransformed;
		float ratio = z_sq / r_sq;
		ratio = ratio > 1 ? 1 : ratio;

		float r = src_r0 + range_r * ratio;
		float g = src_g0 + range_g * ratio;
		float b = src_b0 + range_b * ratio;
		dst_row[i] = GPixel_PackARGB(src_a0, r, g, b);
	}
}

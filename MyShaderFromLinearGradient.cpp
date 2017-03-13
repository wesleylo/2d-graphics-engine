/*
 *  Copyright 2015 Wesley Lo
 */

#include "MyShaderFromLinearGradient.h"

MyShaderFromLinearGradient::MyShaderFromLinearGradient(const GPoint pts[2], const GColor colors[2]) {
	this->pts[0].fX = pts[0].fX;
	this->pts[0].fY = pts[0].fY;
	this->pts[1].fX = pts[1].fX;
	this->pts[1].fY = pts[1].fY;
	this->colors[0] = GColor::MakeARGB(colors[0].fA, colors[0].fR, colors[0].fG, colors[0].fB);
	this->colors[1] = GColor::MakeARGB(colors[1].fA, colors[1].fR, colors[1].fG, colors[1].fB);
}

bool MyShaderFromLinearGradient::setContext(const float ctm[6]) {
	for (int i = 0; i < 6; ++i) {
		this->ctm[i] = ctm[i];
	}

	ptsTransformed[0] = GPoint::Make(pts[0].fX, pts[0].fY);
	ptsTransformed[1] = GPoint::Make(pts[1].fX, pts[1].fY);

	colorsTransformed[0] = GColor::MakeARGB(colors[0].fA, colors[0].fR, colors[0].fG, colors[0].fB);
	colorsTransformed[1] = GColor::MakeARGB(colors[1].fA, colors[1].fR, colors[1].fG, colors[1].fB);

	bool rotated = ctm[1] != 0.0 || ctm[3] != 0.0;
	if (!rotated) {
		float x0Transformed = ctm[0] * pts[0].fX + ctm[1] * pts[0].fY + ctm[2];
		float y0Transformed = ctm[3] * pts[0].fX + ctm[4] * pts[0].fY + ctm[5];
		float x1Transformed = ctm[0] * pts[1].fX + ctm[1] * pts[1].fY + ctm[2];
		float y1Transformed = ctm[3] * pts[1].fX + ctm[4] * pts[1].fY + ctm[5];
		if (x0Transformed < x1Transformed) {
			ptsTransformed[0] = GPoint::Make(x0Transformed, y0Transformed);
			ptsTransformed[1] = GPoint::Make(x1Transformed, y1Transformed);
		} else {
			ptsTransformed[0] = GPoint::Make(x1Transformed, y1Transformed);
			ptsTransformed[1] = GPoint::Make(x0Transformed, y0Transformed);

			colorsTransformed[0] = GColor::MakeARGB(colors[1].fA, colors[1].fR, colors[1].fG, colors[1].fB);
			colorsTransformed[1] = GColor::MakeARGB(colors[0].fA, colors[0].fR, colors[0].fG, colors[0].fB);
		}
	}
	return true;
}

void MyShaderFromLinearGradient::shadeRow(int dst_x, int dst_y, int count, GPixel dst_row[]) {
	float src_a0_01 = colorsTransformed[0].fA < 0 ? 0 : colorsTransformed[0].fA;
	float src_r0 = colorsTransformed[0].fR < 0 ? 0 : colorsTransformed[0].fR * 255;
	float src_g0 = colorsTransformed[0].fG < 0 ? 0 : colorsTransformed[0].fG * 255;
	float src_b0 = colorsTransformed[0].fB < 0 ? 0 : colorsTransformed[0].fB * 255;

	float src_a1_01 = colorsTransformed[1].fA < 0 ? 0 : colorsTransformed[1].fA;
	float src_r1 = colorsTransformed[1].fR < 0 ? 0 : colorsTransformed[1].fR * 255;
	float src_g1 = colorsTransformed[1].fG < 0 ? 0 : colorsTransformed[1].fG * 255;
	float src_b1 = colorsTransformed[1].fB < 0 ? 0 : colorsTransformed[1].fB * 255;

	float delta_a_01 = (src_a1_01 - src_a0_01) / (std::min((int) ptsTransformed[1].fX, dst_x + count) - std::max((int) ptsTransformed[0].fX, dst_x) + 1);
	float delta_r = (src_r1 - src_r0) / (std::min((int) ptsTransformed[1].fX, dst_x + count) - std::max((int) ptsTransformed[0].fX, dst_x) + 1);
	float delta_g = (src_g1 - src_g0) / (std::min((int) ptsTransformed[1].fX, dst_x + count) - std::max((int) ptsTransformed[0].fX, dst_x) + 1);
	float delta_b = (src_b1 - src_b0) / (std::min((int) ptsTransformed[1].fX, dst_x + count) - std::max((int) ptsTransformed[0].fX, dst_x) + 1);

	for (int i = dst_x; i < dst_x + count; ++i) {
		float dst_a_01 = GPixel_GetA(dst_row[i]) / 255;
		float dst_r = GPixel_GetR(dst_row[i]);
		float dst_g = GPixel_GetG(dst_row[i]);
		float dst_b = GPixel_GetB(dst_row[i]);

		float blend_a_01 = src_a0_01 + (dst_a_01 * (1 - src_a0_01));
		int blend_a = blend_a_01 * 255;
		int blend_r = blend_a == 0 ? 0 : ((src_r0 * src_a0_01) + ((dst_r * dst_a_01) * (1 - src_a0_01))) / blend_a_01;
		int blend_g = blend_a == 0 ? 0 : ((src_g0 * src_a0_01) + ((dst_g * dst_a_01) * (1 - src_a0_01))) / blend_a_01;
		int blend_b = blend_a == 0 ? 0 : ((src_b0 * src_a0_01) + ((dst_b * dst_a_01) * (1 - src_a0_01))) / blend_a_01;

		dst_row[i] = GPixel_PackARGB(blend_a, blend_r, blend_g, blend_b);

		if (i > ptsTransformed[1].fX) {
			src_a0_01 = colorsTransformed[1].fA < 0 ? 0 : colorsTransformed[1].fA;
			src_r0 = colorsTransformed[1].fR < 0 ? 0 : colorsTransformed[1].fR * 255;
			src_g0 = colorsTransformed[1].fG < 0 ? 0 : colorsTransformed[1].fG * 255;
			src_b0 = colorsTransformed[1].fB < 0 ? 0 : colorsTransformed[1].fB * 255;
		} else if (i > ptsTransformed[0].fX) {
			src_a0_01 += delta_a_01;
			src_r0 += delta_r;
			src_g0 += delta_g;
			src_b0 += delta_b;
		}
	}
}

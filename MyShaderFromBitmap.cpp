/*
 *  Copyright 2015 Wesley Lo
 */

#include <algorithm>
#include "MyShaderFromBitmap.h"

MyShaderFromBitmap::MyShaderFromBitmap(const GBitmap& bitmap, const float localMatrix[6]) {
	this->bitmap = bitmap;
	for (int i = 0; i < 6; ++i) {
		this->localMatrix[i] = localMatrix[i];
	}
}

bool MyShaderFromBitmap::setContext(const float ctm[6]) {
	for (int i = 0; i < 6; ++i) {
		this->ctm[i] = ctm[i];
	}
	return true;
}

void MyShaderFromBitmap::shadeRow(int dst_x, int dst_y, int count, GPixel dst_row[]) {
	// Apply local matrix to y
	int y = (dst_y - localMatrix[5]) / localMatrix[4];
	// y mirrored
	if (ctm[4] < 0)
		y = bitmap.fHeight - y + bitmap.fHeight * localMatrix[4] + localMatrix[5];

	y = std::max(y, 0);
	y = std::min(y, bitmap.fHeight - 1);

	for (int i = dst_x; i < dst_x + count; ++i) {
		// Apply local matrix to x
		int x = (i - localMatrix[2]) / localMatrix[0];
		// x mirrored
		if (ctm[0] < 0)
			x = bitmap.fWidth - x + bitmap.fWidth * localMatrix[4] + localMatrix[5];

		x = std::max(x, 0);
		x = std::min(x, bitmap.fWidth - 1);

		dst_row[i] = bitmap.getAddr(x, y)[0];
	}
}

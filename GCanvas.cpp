/*
 *  Copyright 2015 Wesley Lo
 */

#include "GCanvas.h"
#include "MyCanvas.h"

GCanvas* GCanvas::Create(const GBitmap& bitmap) {
	if (bitmap.width() <= 0 || bitmap.height() <= 0)
		return NULL;
	else if (bitmap.rowBytes() < bitmap.width() * sizeof(GPixel))
		return NULL;
	else
		return new MyCanvas(bitmap);
}

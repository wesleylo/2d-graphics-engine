/*
 *  Copyright 2015 Wesley Lo
 */

#include "GShader.h"
#include "GPoint.h"
#include "GColor.h"

class MyShaderFromLinearGradient: public GShader {
public:
	MyShaderFromLinearGradient(const GPoint pts[2], const GColor colors[2]);

	/**
	 *  Called before each use, this tells the shader the CTM for the current drawing.
	 *  This returns true if the shader can handle the CTM, and therefore it is valid to call
	 *  shadeRow(). If it cannot handle the CTM, this will return false, and shadeRow()
	 *  should not be called.
	 */
	bool setContext(const float ctm[6]);

	/**
	 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
	 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
	 *  can hold at least [count] entries.
	 */
	void shadeRow(int x, int y, int count, GPixel row[]);

protected:
	GPoint pts[2], ptsTransformed[2];
	GColor colors[2], colorsTransformed[2];
	float ctm[6] = { 1, 0, 0, 0, 1, 0 }; // Initialize ctm to identity matrix
};

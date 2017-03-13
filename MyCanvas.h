/*
 *  Copyright 2015 Wesley Lo
 */

#include "GCanvas.h"
#include "GBitmap.h"
#include "GPoint.h"
#include "GColor.h"
#include "GRect.h"
#include "GShader.h"

class Edge {
public:
	float yMax;
	float xMin;
	float mReciprocal;
	Edge* next;

	Edge(float yMax, float xMin, float mReciprocal, Edge* next);
};

class CTM {
public:
	float matrix[6];
	CTM* next;

	CTM(float matrix[6], CTM* next);
};

class MyCanvas: public GCanvas {
public:
	MyCanvas(const GBitmap&);

	/**
	 *  Fill the entire canvas with the specified color.
	 *
	 *  This completely overwrites any previous colors, it does not blend.
	 */
	void clear(const GColor&);

	/**
	 *  Fill the rectangle with the color.
	 *
	 *  The affected pixels are those whose centers are "contained" inside the rectangle:
	 *      e.g. contained == center > min_edge && center <= max_edge
	 *
	 *  Any area in the rectangle that is outside of the bounds of the canvas is ignored.
	 *
	 *  If the color's alpha is < 1, blend it using SRCOVER blend mode.
	 */
	void fillRect(const GRect&, const GColor&);

	/**
	 *  Scale and translate the bitmap such that it fills the specific rectangle.
	 *
	 *  Any area in the rectangle that is outside of the bounds of the canvas is ignored.
	 *
	 *  If a given pixel in the bitmap is not opaque (e.g. GPixel_GetA() < 255) then blend it
	 *  using SRCOVER blend mode.
	 */
	void fillBitmapRect(const GBitmap&, const GRect&);

	/**
	 *  Fill the convex polygon with the color, following the same "containment" rule as
	 *  rectangles.
	 *
	 *  Any area in the polygon that is outside of the bounds of the canvas is ignored.
	 *
	 *  If the color's alpha is < 1, blend it using SRCOVER blend mode.
	 */
	void fillConvexPolygon(const GPoint[], int count, const GColor&);

	/**
	 *  Saves a copy of the CTM, allowing subsequent modifications (by calling concat()) to be
	 *  undone when restore() is called.
	 */
	void save();

	/**
	 *  Balances calls to save(), returning the CTM to the state it was in when the corresponding
	 *  call to save() was made. These calls can be nested.
	 */
	void restore();

	/**
	 *  Modifies the CTM (current transformation matrix) by pre-concatenating it with the specfied
	 *  matrix.
	 *
	 *  CTM' = CTM * matrix
	 *
	 *  The result is that any drawing that uses the new CTM' will be affected AS-IF it were
	 *  first transformed by matrix, and then transformed by the previous CTM.
	 */
	void concat(const float matrix[6]);

	/**
	 *  Fill the specified rect using the shader. The colors returned by the shader are blended
	 *  into the canvas using SRC_OVER blend mode.
	 */
	void shadeRect(const GRect& rect, GShader* shader);

	/**
	 *  Fill the specified polygon using the shader. The colors returned by the shader are blended
	 *  into the canvas using SRC_OVER blend mode.
	 */
	void shadeConvexPolygon(const GPoint[], int count, GShader* shader);

	/**
	 *  Stroke the specified polygon using the Stroke settings. If isClosed is true, then the
	 *  drawn stroke should connect the first and last points of the polygon, else it should not,
	 *  and those end-caps should reflect the Stroke.fAddCap setting.
	 */
	void strokePolygon(const GPoint[], int count, bool isClosed, const Stroke&, GShader*);

protected:
	GBitmap dst;
	float ctm[6] = { 1, 0, 0, 0, 1, 0 }; // Initialize ctm to identity matrix
	CTM* ctmStack = NULL; // Initialize ctm stack to be empty

	void fillPoint(GPoint point, GPixel pixel);

	void fillLine(float x1, float x2, int y, const GColor& color);

	void fillLine(float x1, float x2, int y, GShader* shader);

	void fillStroke(float x1, float x2, int y, GShader* shader);

	void transformPoints(const GPoint[], GPoint[], int count);

	void transformRect(const GRect& rectUntransformed, GRect& rect);

	void shadeStrokePolygon(const GPoint[], int count, GShader* shader);
};

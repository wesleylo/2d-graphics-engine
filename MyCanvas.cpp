/*
 *  Copyright 2015 Wesley Lo
 */

#include "MyCanvas.h"

Edge::Edge(float yMax, float xMin, float mReciprocal, Edge* next) {
	this->yMax = yMax;
	this->xMin = xMin;
	this->mReciprocal = mReciprocal;
	this->next = next;
}

CTM::CTM(float matrix[6], CTM* next) {
	for (int i = 0; i < 6; ++i) {
		this->matrix[i] = matrix[i];
	}
	this->next = next;
}

MyCanvas::MyCanvas(const GBitmap& bitmap) {
	dst = bitmap;
}

void MyCanvas::clear(const GColor& color) {
	float fA = color.fA < 0 ? 0 : color.fA * 255.9999f;
	int a = fA;
	int r = color.fR < 0 ? 0 : color.fR * fA;
	int g = color.fG < 0 ? 0 : color.fG * fA;
	int b = color.fB < 0 ? 0 : color.fB * fA;

	GPixel* row = dst.pixels();
	for (int y = 0; y < dst.height(); ++y) {
		for (int x = 0; x < dst.width(); ++x) {
			// Fill the entire canvas with the specified color, using SRC port-duff mode.
			row[x] = GPixel_PackARGB(a, r, g, b);
		}
		row += dst.rowBytes() >> 2;
	}
}

void MyCanvas::fillPoint(GPoint point, GPixel pixel) {
	int dst_x = floor(point.fX + 0.5);
	int dst_y = floor(point.fY + 0.5);

	if (dst_x < 0 || dst_x >= dst.fWidth || dst_y < 0 || dst_y >= dst.fHeight)
		return;

	float src_a_01 = GPixel_GetA(pixel) / 255.0;
	float src_r = GPixel_GetR(pixel);
	float src_g = GPixel_GetG(pixel);
	float src_b = GPixel_GetB(pixel);

	GPixel* dst_row = dst.pixels();
	for (int i = 0; i < dst_y; ++i) {
		dst_row += dst.rowBytes() >> 2;
	}

	float dst_a_01 = GPixel_GetA(dst_row[dst_x]) / 255.0;
	float dst_r = GPixel_GetR(dst_row[dst_x]);
	float dst_g = GPixel_GetG(dst_row[dst_x]);
	float dst_b = GPixel_GetB(dst_row[dst_x]);

	float blend_a_01 = src_a_01 + (dst_a_01 * (1 - src_a_01));
	unsigned blend_a = blend_a_01 * 255;
	float blend_r = blend_a == 0 ? 0 : ((src_r * src_a_01) + ((dst_r * dst_a_01) * (1 - src_a_01))) / blend_a_01;
	float blend_g = blend_a == 0 ? 0 : ((src_g * src_a_01) + ((dst_g * dst_a_01) * (1 - src_a_01))) / blend_a_01;
	float blend_b = blend_a == 0 ? 0 : ((src_b * src_a_01) + ((dst_b * dst_a_01) * (1 - src_a_01))) / blend_a_01;

	if (round(blend_r) > blend_a)
		blend_r = blend_a;
	if (round(blend_g) > blend_a)
		blend_g = blend_a;
	if (round(blend_b) > blend_a)
		blend_b = blend_a;

	dst_row[dst_x] = GPixel_PackARGB(round(blend_a), round(blend_r), round(blend_g), round(blend_b));
}

void MyCanvas::fillLine(float x1, float x2, int y, const GColor& color) {
	int left = floor(x1 + 0.5);
	int right = floor(x2 + 0.5);

	if (y < 0 || y >= dst.fHeight)
		return;

	if (left < 0)
		left = 0;

	if (right >= dst.fWidth)
		right = dst.fWidth;

	float src_a_01 = color.fA < 0 ? 0 : color.fA;
	float src_r = color.fR < 0 ? 0 : color.fR * 255;
	float src_g = color.fG < 0 ? 0 : color.fG * 255;
	float src_b = color.fB < 0 ? 0 : color.fB * 255;

	GPixel* dst_row = dst.pixels();
	for (int i = 0; i < y; ++i) {
		dst_row += dst.rowBytes() >> 2;
	}

	for (int dst_x = left; dst_x < right; ++dst_x) {
		float dst_a_01 = GPixel_GetA(dst_row[dst_x]) / 255.0;
		float dst_r = GPixel_GetR(dst_row[dst_x]);
		float dst_g = GPixel_GetG(dst_row[dst_x]);
		float dst_b = GPixel_GetB(dst_row[dst_x]);

		float blend_a_01 = src_a_01 + (dst_a_01 * (1 - src_a_01));
		unsigned blend_a = blend_a_01 * 255;
		float blend_r = blend_a == 0 ? 0 : ((src_r * src_a_01) + ((dst_r * dst_a_01) * (1 - src_a_01))) / blend_a_01;
		float blend_g = blend_a == 0 ? 0 : ((src_g * src_a_01) + ((dst_g * dst_a_01) * (1 - src_a_01))) / blend_a_01;
		float blend_b = blend_a == 0 ? 0 : ((src_b * src_a_01) + ((dst_b * dst_a_01) * (1 - src_a_01))) / blend_a_01;

		if (round(blend_r) > blend_a)
			blend_r = blend_a;
		if (round(blend_g) > blend_a)
			blend_g = blend_a;
		if (round(blend_b) > blend_a)
			blend_b = blend_a;

		dst_row[dst_x] = GPixel_PackARGB(round(blend_a), round(blend_r), round(blend_g), round(blend_b));
	}
}

void MyCanvas::fillLine(float x1, float x2, int y, GShader* shader) {
	int left = floor(x1 + 0.5);
	int right = floor(x2 + 0.5);

	if (y < 0 || y >= dst.fHeight)
		return;

	if (left < 0)
		left = 0;

	if (right >= dst.fWidth)
		right = dst.fWidth;

	GPixel* dst_row = dst.pixels();
	for (int i = 0; i < y; ++i) {
		dst_row += dst.rowBytes() >> 2;
	}

	shader->setContext(ctm);
	shader->shadeRow(left, y, right - left + 1, dst_row);
}

void MyCanvas::fillStroke(float x1, float x2, int y, GShader* shader) {
	int left = floor(x1 + 0.5);
	int right = floor(x2 + 0.5);

	if (y < 0 || y >= dst.fHeight)
		return;

	if (left < 0)
		left = 0;

	if (right >= dst.fWidth)
		right = dst.fWidth;

	GPixel* dst_row = dst.pixels();
	for (int i = 0; i < y; ++i) {
		dst_row += dst.rowBytes() >> 2;
	}
	dst_row = dst_row + (unsigned int) left;

	shader->setContext(ctm);
	shader->shadeRow(0, y, right - left + 1, dst_row);
}

void MyCanvas::fillRect(const GRect& rect, const GColor& color) {
	int left = floor(rect.fLeft + 0.5);
	int top = floor(rect.fTop + 0.5);
	int right = floor(rect.fRight + 0.5);
	int bottom = floor(rect.fBottom + 0.5);

	float src_a_01 = color.fA < 0 ? 0 : color.fA;
	float src_r = color.fR < 0 ? 0 : color.fR * 255;
	float src_g = color.fG < 0 ? 0 : color.fG * 255;
	float src_b = color.fB < 0 ? 0 : color.fB * 255;

	GPixel* dst_row = dst.pixels();

	for (int dst_y = 0; dst_y < dst.height(); ++dst_y) {
		for (int dst_x = 0; dst_x < dst.width(); ++dst_x) {
			if (dst_x >= left && dst_x < right && dst_y >= top && dst_y < bottom) {
				float dst_a_01 = GPixel_GetA(dst_row[dst_x]) / 255.0;
				float dst_r = GPixel_GetR(dst_row[dst_x]);
				float dst_g = GPixel_GetG(dst_row[dst_x]);
				float dst_b = GPixel_GetB(dst_row[dst_x]);

				float blend_a_01 = src_a_01 + (dst_a_01 * (1 - src_a_01));
				unsigned blend_a = blend_a_01 * 255;
				float blend_r = blend_a == 0 ? 0 : ((src_r * src_a_01) + ((dst_r * dst_a_01) * (1 - src_a_01))) / blend_a_01;
				float blend_g = blend_a == 0 ? 0 : ((src_g * src_a_01) + ((dst_g * dst_a_01) * (1 - src_a_01))) / blend_a_01;
				float blend_b = blend_a == 0 ? 0 : ((src_b * src_a_01) + ((dst_b * dst_a_01) * (1 - src_a_01))) / blend_a_01;

				dst_row[dst_x] = GPixel_PackARGB(round(blend_a), round(blend_r), round(blend_g), round(blend_b));
			}
		}
		// Advance dst_row pointer to the next row
		dst_row += dst.rowBytes() >> 2;
	}
}

void MyCanvas::fillBitmapRect(const GBitmap& src, const GRect& rectUntransformed) {
	GRect rect;
	bool rotated = ctm[1] != 0.0 || ctm[3] != 0.0;
	float loop_increment = 1;

	if (rotated) {
		rect.fLeft = rectUntransformed.fLeft;
		rect.fTop = rectUntransformed.fTop;
		rect.fRight = rectUntransformed.fRight;
		rect.fBottom = rectUntransformed.fBottom;
		loop_increment = 0.4;
	} else {
		transformRect(rectUntransformed, rect);
	}

	int left = floor(rect.fLeft + 0.5);
	int top = floor(rect.fTop + 0.5);
	int right = floor(rect.fRight + 0.5);
	int bottom = floor(rect.fBottom + 0.5);

	GPixel* src_row = src.pixels();
	int src_y_scaled = 0;

	if (!rotated) {
		if (top < 0) { 	// When the top of rect is off the canvas, we need to advance src_row pointer to the correct row
			// Scale src height
			src_y_scaled = (0 - top + 1) * src.height() / (bottom - top + 1);
		} else { // Advance src_row pointer to the first scaled row
			// Scale src height
			src_y_scaled = 1 * src.height() / (bottom - top + 1);
		}

		// Advance src_row pointer to the correct row
		src_row = src.pixels();
		for (int z = 0; z < src_y_scaled; ++z) {
			src_row += src.rowBytes() >> 2;
		}
	}

	for (float dst_y = top; dst_y < bottom; ++dst_y) {
		for (float dst_x = left; dst_x < right; dst_x += loop_increment) {
			GPoint pointUntransformed[1] = { dst_x, dst_y };
			GPoint point[1] = { dst_x, dst_y };

			if (rotated) {
				transformPoints(pointUntransformed, point, 1);
			}

			// Scale src width
			int src_x_scaled = (dst_x - left + 1) * src.width() / (right - left + 1);

			fillPoint(point[0], src_row[src_x_scaled]);
		}
		// Scale src height
		src_y_scaled = (dst_y - top + 2) * src.height() / (bottom - top + 1);

		// Advance src_row pointer to the correct row
		src_row = src.pixels();
		for (int z = 0; z < src_y_scaled; ++z) {
			src_row += src.rowBytes() >> 2;
		}
	}
}

void MyCanvas::fillConvexPolygon(const GPoint pointsUntransformed[], int count, const GColor& color) {
	// Polygon must have at least 3 points
	if (count < 3)
		return;

	GPoint points[count];
	transformPoints(pointsUntransformed, points, count);

	float yMaxGlobal = round(points[0].fY);
	float yMinGlobal = round(points[0].fY);

	// Compute global edge table size and array offset
	for (int i = 0; i < count; ++i) {
		if (points[i].fY < yMinGlobal)
			yMinGlobal = round(points[i].fY);
		if (points[i].fY > yMaxGlobal)
			yMaxGlobal = round(points[i].fY);
	}

	int globalEdgeTableSize = yMinGlobal < 0 ? yMaxGlobal - yMinGlobal + 1 : yMaxGlobal + 1;
	int arrayOffset = yMinGlobal < 0 ? abs(yMinGlobal) : 0;
	int y = yMinGlobal;

	Edge* globalEdgeTable[globalEdgeTableSize];
	Edge* activeEdgeTable = NULL;
	Edge* prevPtr = NULL;
	Edge* currPtr = NULL;

	// Initialize global edge table
	for (int i = 0; i < globalEdgeTableSize; ++i) {
		globalEdgeTable[i] = NULL;
	}

	// Create global edge table
	for (int i = 0; i < count; ++i) {
		if (points[(i + 1) % count].fY - points[i].fY != 0) { // slope != 0
			float yMax = 0;
			float yMin = 0;
			float xMin = 0;
			if (points[(i + 1) % count].fY > points[i].fY) {
				yMax = round(points[(i + 1) % count].fY);
				xMin = round(points[i].fX);
				yMin = round(points[i].fY);
			} else {
				yMax = round(points[i].fY);
				xMin = round(points[(i + 1) % count].fX);
				yMin = round(points[(i + 1) % count].fY);
			}
			float mReciprocal = (points[(i + 1) % count].fX - points[i].fX) / (points[(i + 1) % count].fY - points[i].fY);

			globalEdgeTable[(int) yMin + arrayOffset] = new Edge(yMax, xMin, mReciprocal, globalEdgeTable[(int) yMin + arrayOffset]);
		}
	}

	do {
		// Move edges to active edge table
		while (globalEdgeTable[y + arrayOffset] != NULL) {
			if (activeEdgeTable == NULL) {
				activeEdgeTable = globalEdgeTable[y + arrayOffset];
				globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
				activeEdgeTable->next = NULL;
			} else {
				// If the xMin's on the current line are the same, compare the xMin's on the next line
				float globalXMin = globalEdgeTable[y + arrayOffset]->xMin;
				float activeXMin = activeEdgeTable->xMin;
				if (globalXMin == activeEdgeTable->xMin) {
					globalXMin += globalEdgeTable[y + arrayOffset]->mReciprocal;
					activeXMin += activeEdgeTable->mReciprocal;
				}

				if (globalXMin < activeXMin) {
					currPtr = activeEdgeTable;
					activeEdgeTable = globalEdgeTable[y + arrayOffset];
					globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
					activeEdgeTable->next = currPtr;
				} else {
					prevPtr = activeEdgeTable;
					currPtr = activeEdgeTable->next;
					while (currPtr != NULL && globalEdgeTable[y + arrayOffset]->xMin >= currPtr->xMin) {
						prevPtr = prevPtr->next;
						currPtr = currPtr->next;
					}
					prevPtr->next = globalEdgeTable[y + arrayOffset];
					globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
					prevPtr->next->next = currPtr;
				}
			}
		}

		// Remove finished edges
		prevPtr = activeEdgeTable;
		currPtr = activeEdgeTable;
		while (currPtr != NULL) {
			if ((int) round(currPtr->yMax) == y) {
				if (activeEdgeTable == currPtr) {
					activeEdgeTable = currPtr->next;
					currPtr = currPtr->next;
				} else {
					prevPtr->next = currPtr->next;
					currPtr = currPtr->next;
				}
			} else {
				prevPtr = currPtr;
				currPtr = currPtr->next;
			}
		}

		if (activeEdgeTable != NULL) {
			// Fill the line
			fillLine(activeEdgeTable->xMin, activeEdgeTable->next->xMin, y, color);

			// Update x for all edges in active edge table
			currPtr = activeEdgeTable;
			while (currPtr != NULL) {
				currPtr->xMin += currPtr->mReciprocal;
				currPtr = currPtr->next;
			}

			// Go to next scan line
			y++;
		}
	} while (activeEdgeTable != NULL);
}

void MyCanvas::transformPoints(const GPoint pointsUntransformed[], GPoint points[], int count) {
	for (int i = 0; i < count; ++i) {
		float transformedX = ctm[0] * pointsUntransformed[i].fX + ctm[1] * pointsUntransformed[i].fY + ctm[2];
		float transformedY = ctm[3] * pointsUntransformed[i].fX + ctm[4] * pointsUntransformed[i].fY + ctm[5];
		points[i] = GPoint::Make(transformedX, transformedY);
	}
}

void MyCanvas::transformRect(const GRect& rectUntransformed, GRect& rect) {
	// Apply matrix to a point
	// [ A B C ][ x ]   [ Ax + By + C ]
	// [ D E F ][ y ] = [ Dx + Ey + F ]
	// [ 0 0 1 ][ 1 ]   [      1      ]

	float transformedLeft = ctm[0] * rectUntransformed.fLeft + ctm[1] * rectUntransformed.fTop + ctm[2];
	float transformedTop = ctm[3] * rectUntransformed.fLeft + ctm[4] * rectUntransformed.fTop + ctm[5];
	float transformedRight = ctm[0] * rectUntransformed.fRight + ctm[1] * rectUntransformed.fBottom + ctm[2];
	float transformedBottom = ctm[3] * rectUntransformed.fRight + ctm[4] * rectUntransformed.fBottom + ctm[5];
	rect = GRect::MakeLTRB(transformedLeft, transformedTop, transformedRight, transformedBottom);
}

void MyCanvas::save() {
	ctmStack = new CTM(ctm, ctmStack);
}

void MyCanvas::restore() {
	for (int i = 0; i < 6; ++i) {
		ctm[i] = ctmStack->matrix[i];
	}
	ctmStack = ctmStack->next;
}

void MyCanvas::concat(const float matrix[6]) {
	float concatMatrix[6];

	// Matrix multiplication
	// [ A B C ] [ a b c ]   [ Aa+Bd  Ab+Be  Ac+Bf+C ]
	// [ D E F ]*[ d e f ] = [ Da+Ed  Db+Ee  Dc+Ef+F ]
	// [ 0 0 1 ] [ 0 0 1 ]   [   0      0       1    ]

	concatMatrix[0] = ctm[0] * matrix[0] + ctm[1] * matrix[3];
	concatMatrix[1] = ctm[0] * matrix[1] + ctm[1] * matrix[4];
	concatMatrix[2] = ctm[0] * matrix[2] + ctm[1] * matrix[5] + ctm[2];
	concatMatrix[3] = ctm[3] * matrix[0] + ctm[4] * matrix[3];
	concatMatrix[4] = ctm[3] * matrix[1] + ctm[4] * matrix[4];
	concatMatrix[5] = ctm[3] * matrix[2] + ctm[4] * matrix[5] + ctm[5];

	for (int i = 0; i < 6; ++i) {
		ctm[i] = concatMatrix[i];
	}
}

void MyCanvas::shadeRect(const GRect& rectUntransformed, GShader* shader) {
	GRect rect;
	bool rotated = ctm[1] != 0.0 || ctm[3] != 0.0;
	bool scaled = ctm[0] != 1.0 || ctm[4] != 1.0;

	if (rotated || !scaled) {
		rect.fLeft = rectUntransformed.fLeft;
		rect.fTop = rectUntransformed.fTop;
		rect.fRight = rectUntransformed.fRight;
		rect.fBottom = rectUntransformed.fBottom;
	} else {
		transformRect(rectUntransformed, rect);
	}

	int left = std::max(0, (int) floor(rect.fLeft + 0.5));
	int top = std::max(0, (int) floor(rect.fTop + 0.5));
	int right = std::min(dst.fWidth, (int) floor(rect.fRight + 0.5));
	int bottom = std::min(dst.fHeight, (int) floor(rect.fBottom + 0.5));

	GPixel* dst_row = dst.pixels();
	for (int z = 0; z < top; ++z) {
		dst_row += dst.rowBytes() >> 2;
	}

	for (int dst_y = top; dst_y < bottom; ++dst_y) {
		GPixel src_row[1024];
		shader->setContext(ctm);
		shader->shadeRow(left, dst_y, right - left + 1, src_row);

		for (int i = left; i < right; ++i) {
			GPoint pointUntransformed[1] = { i, dst_y };
			GPoint point[1] = { i, dst_y };

			if (rotated || !scaled) {
				transformPoints(pointUntransformed, point, 1);
			}

			fillPoint(point[0], src_row[i]);
		}

		dst_row += dst.rowBytes() >> 2;
	}
}

void MyCanvas::shadeConvexPolygon(const GPoint pointsUntransformed[], int count, GShader* shader) {
	// Polygon must have at least 3 points
	if (count < 3)
		return;

	GPoint points[count];
	transformPoints(pointsUntransformed, points, count);

	float yMaxGlobal = round(points[0].fY);
	float yMinGlobal = round(points[0].fY);

	// Compute global edge table size and array offset
	for (int i = 0; i < count; ++i) {
		if (points[i].fY < yMinGlobal)
			yMinGlobal = round(points[i].fY);
		if (points[i].fY > yMaxGlobal)
			yMaxGlobal = round(points[i].fY);
	}

	int globalEdgeTableSize = yMinGlobal < 0 ? yMaxGlobal - yMinGlobal + 1 : yMaxGlobal + 1;
	int arrayOffset = yMinGlobal < 0 ? abs(yMinGlobal) : 0;
	int y = yMinGlobal;

	Edge* globalEdgeTable[globalEdgeTableSize];
	Edge* activeEdgeTable = NULL;
	Edge* prevPtr = NULL;
	Edge* currPtr = NULL;

	// Initialize global edge table
	for (int i = 0; i < globalEdgeTableSize; ++i) {
		globalEdgeTable[i] = NULL;
	}

	// Create global edge table
	for (int i = 0; i < count; ++i) {
		if (points[(i + 1) % count].fY - points[i].fY != 0) { // slope != 0
			float yMax = 0;
			float yMin = 0;
			float xMin = 0;
			if (points[(i + 1) % count].fY > points[i].fY) {
				yMax = round(points[(i + 1) % count].fY);
				xMin = round(points[i].fX);
				yMin = round(points[i].fY);
			} else {
				yMax = round(points[i].fY);
				xMin = round(points[(i + 1) % count].fX);
				yMin = round(points[(i + 1) % count].fY);
			}
			float mReciprocal = (points[(i + 1) % count].fX - points[i].fX) / (points[(i + 1) % count].fY - points[i].fY);

			globalEdgeTable[(int) yMin + arrayOffset] = new Edge(yMax, xMin, mReciprocal, globalEdgeTable[(int) yMin + arrayOffset]);
		}
	}

	do {
		// Move edges to active edge table
		while (globalEdgeTable[y + arrayOffset] != NULL) {
			if (activeEdgeTable == NULL) {
				activeEdgeTable = globalEdgeTable[y + arrayOffset];
				globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
				activeEdgeTable->next = NULL;
			} else {
				// If the xMin's on the current line are the same, compare the xMin's on the next line
				float globalXMin = globalEdgeTable[y + arrayOffset]->xMin;
				float activeXMin = activeEdgeTable->xMin;
				if (globalXMin == activeEdgeTable->xMin) {
					globalXMin += globalEdgeTable[y + arrayOffset]->mReciprocal;
					activeXMin += activeEdgeTable->mReciprocal;
				}

				if (globalXMin < activeXMin) {
					currPtr = activeEdgeTable;
					activeEdgeTable = globalEdgeTable[y + arrayOffset];
					globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
					activeEdgeTable->next = currPtr;
				} else {
					prevPtr = activeEdgeTable;
					currPtr = activeEdgeTable->next;
					while (currPtr != NULL && globalEdgeTable[y + arrayOffset]->xMin >= currPtr->xMin) {
						prevPtr = prevPtr->next;
						currPtr = currPtr->next;
					}
					prevPtr->next = globalEdgeTable[y + arrayOffset];
					globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
					prevPtr->next->next = currPtr;
				}
			}
		}

		// Remove finished edges
		prevPtr = activeEdgeTable;
		currPtr = activeEdgeTable;
		while (currPtr != NULL) {
			if ((int) round(currPtr->yMax) == y) {
				if (activeEdgeTable == currPtr) {
					activeEdgeTable = currPtr->next;
					currPtr = currPtr->next;
				} else {
					prevPtr->next = currPtr->next;
					currPtr = currPtr->next;
				}
			} else {
				prevPtr = currPtr;
				currPtr = currPtr->next;
			}
		}

		if (activeEdgeTable != NULL) {
			// Fill the line
			fillLine(activeEdgeTable->xMin, activeEdgeTable->next->xMin, y, shader);

			// Update x for all edges in active edge table
			currPtr = activeEdgeTable;
			while (currPtr != NULL) {
				currPtr->xMin += currPtr->mReciprocal;
				currPtr = currPtr->next;
			}

			// Go to next scan line
			y++;
		}
	} while (activeEdgeTable != NULL);
}

void MyCanvas::shadeStrokePolygon(const GPoint pointsUntransformed[], int count, GShader* shader) {
	// Polygon must have at least 3 points
	if (count < 3)
		return;

	GPoint points[count];
	transformPoints(pointsUntransformed, points, count);

	float yMaxGlobal = round(points[0].fY);
	float yMinGlobal = round(points[0].fY);

	// Compute global edge table size and array offset
	for (int i = 0; i < count; ++i) {
		if (points[i].fY < yMinGlobal)
			yMinGlobal = round(points[i].fY);
		if (points[i].fY > yMaxGlobal)
			yMaxGlobal = round(points[i].fY);
	}

	int globalEdgeTableSize = yMinGlobal < 0 ? yMaxGlobal - yMinGlobal + 1 : yMaxGlobal + 1;
	int arrayOffset = yMinGlobal < 0 ? abs(yMinGlobal) : 0;
	int y = yMinGlobal;

	Edge* globalEdgeTable[globalEdgeTableSize];
	Edge* activeEdgeTable = NULL;
	Edge* prevPtr = NULL;
	Edge* currPtr = NULL;

	// Initialize global edge table
	for (int i = 0; i < globalEdgeTableSize; ++i) {
		globalEdgeTable[i] = NULL;
	}

	// Create global edge table
	for (int i = 0; i < count; ++i) {
		if (points[(i + 1) % count].fY - points[i].fY != 0) { // slope != 0
			float yMax = 0;
			float yMin = 0;
			float xMin = 0;
			if (points[(i + 1) % count].fY > points[i].fY) {
				yMax = round(points[(i + 1) % count].fY);
				xMin = round(points[i].fX);
				yMin = round(points[i].fY);
			} else {
				yMax = round(points[i].fY);
				xMin = round(points[(i + 1) % count].fX);
				yMin = round(points[(i + 1) % count].fY);
			}
			float mReciprocal = (points[(i + 1) % count].fX - points[i].fX) / (points[(i + 1) % count].fY - points[i].fY);

			globalEdgeTable[(int) yMin + arrayOffset] = new Edge(yMax, xMin, mReciprocal, globalEdgeTable[(int) yMin + arrayOffset]);
		}
	}

	do {
		// Move edges to active edge table
		while (globalEdgeTable[y + arrayOffset] != NULL) {
			if (activeEdgeTable == NULL) {
				activeEdgeTable = globalEdgeTable[y + arrayOffset];
				globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
				activeEdgeTable->next = NULL;
			} else {
				// If the xMin's on the current line are the same, compare the xMin's on the next line
				float globalXMin = globalEdgeTable[y + arrayOffset]->xMin;
				float activeXMin = activeEdgeTable->xMin;
				if (globalXMin == activeEdgeTable->xMin) {
					globalXMin += globalEdgeTable[y + arrayOffset]->mReciprocal;
					activeXMin += activeEdgeTable->mReciprocal;
				}

				if (globalXMin < activeXMin) {
					currPtr = activeEdgeTable;
					activeEdgeTable = globalEdgeTable[y + arrayOffset];
					globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
					activeEdgeTable->next = currPtr;
				} else {
					prevPtr = activeEdgeTable;
					currPtr = activeEdgeTable->next;
					while (currPtr != NULL && globalEdgeTable[y + arrayOffset]->xMin >= currPtr->xMin) {
						prevPtr = prevPtr->next;
						currPtr = currPtr->next;
					}
					prevPtr->next = globalEdgeTable[y + arrayOffset];
					globalEdgeTable[y + arrayOffset] = globalEdgeTable[y + arrayOffset]->next;
					prevPtr->next->next = currPtr;
				}
			}
		}

		// Remove finished edges
		prevPtr = activeEdgeTable;
		currPtr = activeEdgeTable;
		while (currPtr != NULL) {
			if ((int) round(currPtr->yMax) == y) {
				if (activeEdgeTable == currPtr) {
					activeEdgeTable = currPtr->next;
					currPtr = currPtr->next;
				} else {
					prevPtr->next = currPtr->next;
					currPtr = currPtr->next;
				}
			} else {
				prevPtr = currPtr;
				currPtr = currPtr->next;
			}
		}

		if (activeEdgeTable != NULL) {
			// Fill the line
			fillStroke(activeEdgeTable->xMin, activeEdgeTable->next->xMin, y, shader);

			// Update x for all edges in active edge table
			currPtr = activeEdgeTable;
			while (currPtr != NULL) {
				currPtr->xMin += currPtr->mReciprocal;
				currPtr = currPtr->next;
			}

			// Go to next scan line
			y++;
		}
	} while (activeEdgeTable != NULL);
}

void MyCanvas::strokePolygon(const GPoint points[], int pointCount, bool isClosed, const Stroke& stroke, GShader* shader) {
	// Line must have at least 2 points
	if (pointCount < 2)
		return;

	GPoint notchPolygonPoints[4];
	GPoint lastBevelPolygonPoint;
	float prevX, prevY, prevLen, prevXPrime, prevYPrime = 0;
	int loopCount = isClosed ? pointCount + 1 : pointCount - 1;
	for (int i = 0; i < loopCount; ++i) {
		float xA = points[i % pointCount].fX;
		float yA = points[i % pointCount].fY;
		float xB = points[(i + 1) % pointCount].fX;
		float yB = points[(i + 1) % pointCount].fY;

		// compute xPrime & yPrime for quad polygon
		float x = xB - xA;
		float y = yB - yA;
		float len = sqrt(x * x + y * y);
		float rad = stroke.fWidth / 2;
		float xPrime = x * rad / len;
		float yPrime = y * rad / len;

		// adjust for end caps
		if (stroke.fAddCap) {
			xA -= xPrime;
			yA -= yPrime;
			xB += xPrime;
			yB += yPrime;

			// compute xPrime & yPrime for quad polygon again
			x = xB - xA;
			y = yB - yA;
			len = sqrt(x * x + y * y);
			rad = stroke.fWidth / 2;
			xPrime = x * rad / len;
			yPrime = y * rad / len;
		}

		// draw quad polygon
		GPoint quadPolygonPoints[4];
		quadPolygonPoints[0] = GPoint::Make(xA - yPrime, yA + xPrime);
		quadPolygonPoints[1] = GPoint::Make(xA + yPrime, yA - xPrime);
		quadPolygonPoints[2] = GPoint::Make(xB + yPrime, yB - xPrime);
		quadPolygonPoints[3] = GPoint::Make(xB - yPrime, yB + xPrime);
		shadeStrokePolygon(quadPolygonPoints, 4, shader);

		// compute join style
		float h = abs(prevX) / prevLen + abs(prevY) / prevLen + abs(x) / len + abs(y) / len;
		int notchPolygonPointCount = h > stroke.fMiterLimit || h == 0 || stroke.fMiterLimit == 3.00 ? 3 : 4;

		// draw notch polygon
		if (i == 0) {
			lastBevelPolygonPoint = GPoint::Make(xA + yPrime, yA - xPrime);
		} else if ((i < loopCount) || (i == loopCount && isClosed)) {
			notchPolygonPoints[0] = GPoint::Make(xA + yPrime, yA - xPrime);
			notchPolygonPoints[3] = GPoint::Make(xA + yPrime + prevYPrime, yA - xPrime - prevXPrime);
			shadeStrokePolygon(notchPolygonPoints, notchPolygonPointCount, shader);
		}

		// save info for next loop
		notchPolygonPoints[1] = GPoint::Make(xB, yB);
		notchPolygonPoints[2] = GPoint::Make(xB + yPrime, yB - xPrime);
		prevX = x;
		prevY = y;
		prevLen = len;
		prevXPrime = xPrime;
		prevYPrime = yPrime;
	}
}

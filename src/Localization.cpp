#include "../include/GeneralProblems.h"
#include <cmath>

SegmentLocalization findInSegment(vec2 point, vec2 a, vec2 b){
	vec2 segment = b - a;
	vec2 pointDiff = point - a;
	float crossZ = pointDiff.x() * segment.y() - pointDiff.y() * segment.x();
	if (crossZ > 0)
		return RIGHT;
	if (crossZ < 0)
		return LEFT;
	
	float xP, xA, xB;
	if (b.y() != a.y())
		xP = point.x(), xA = a.x(), xB = b.x();
	 else 
		xP = point.y(), xA = a.y(), xB = b.y();

	if (xP < xA)
		return BEFORE_A;
	if (xP == xA)
		return ON_A;
	if (xP < xB)
		return BETWEEN_A_B;
	if (xP == xB)
		return ON_B;
	return AFTER_B;
}

float det(float m[9]){
	return m[0] * m[4] * m[8] +
		m[1] * m[5] * m[6] +
		m[2] * m[3] * m[7] -
		m[2] * m[4] * m[6] -
		m[5] * m[7] * m[0] -
		m[1] * m[3] * m[8];
}

unsigned int getSign(float f){
	if (f >= 0)
		return 1;
	return 0;
}

TriangleLocalization findInTriangle(vec2 point, vec2 p1, vec2 p2, vec2 p3){
	float mat[9] = {
		p1.x(), p2.x(), p3.x(),
		p1.y(), p2.y(), p3.y(),
		1,1,1
	};
	float delta = det(mat);
	mat[0] = point.x();
	mat[3] = point.y();
	float l1 = det(mat) / delta;
	mat[0] = p1.x(), mat[3] = p1.y();
	mat[1] = point.x(), mat[4] = point.y();
	float l2 = det(mat) / delta;
	mat[1] = p2.x(), mat[4] = p2.y();
	mat[2] = point.x(), mat[5] = point.y();
	float l3 = det(mat) / delta;

	int sign = (getSign(l1) << 2) | (getSign(l2) << 1) | getSign(l3);

	return (TriangleLocalization)sign;
}

bool segmentIntersects(vec2 a, vec2 b, vec2 c, vec2 d){
	return ((c - a).crossSign(b - a) != (d - a).crossSign(b - a)) &&
		((a - c).crossSign(d - c) != (b - c).crossSign(d - c));
}

static float getHighestX(vec2* points, int count){
	float highest = points[0].x();
	for (int i = 1; i < count; i++){
		if (points[i].x()> highest)
			highest = points[i].x();
	}
	return highest;
}

bool findInPolygon(vec2 point, vec2* polygonPoints, int count){
	int intersectionCounts = 0;
	float highestX = getHighestX(polygonPoints, count);
	vec2 bound = vec2(highestX + 10, point.y());
	for (int i = 0; i < count; i++){
		vec2 p1 = polygonPoints[i], p2 = polygonPoints[(i + 1) % count];
		if (segmentIntersects(p1, p2, point, bound))
			intersectionCounts++;
	}
	return intersectionCounts % 2 == 1;
}
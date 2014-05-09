#ifndef GC_PROBLEMS_H
#define GC_PROBLEMS_H

#include <vector>
#include "../include/angel.h"

std::vector<vec2> getHull(vec2 * points, int count);

enum SegmentLocalization { RIGHT, LEFT, BEFORE_A, ON_A, BETWEEN_A_B, ON_B, AFTER_B };
SegmentLocalization findInSegment(vec2 point, vec2 a, vec2 b);

enum TriangleLocalization { INSIDE = 7, P1_AREA = 4, P2_AREA = 2, P3_AREA = 1, P1P2_AREA = 6, P2P3_AREA = 3, P1P3_AREA = 5};
TriangleLocalization findInTriangle(vec2 point, vec2 p1, vec2 p2, vec2 p3);

bool findInPolygon(vec2 point, vec2* polygonPoints, int count);

bool segmentIntersects(vec2 seg1Start, vec2 seg1End, vec2 seg2Start, vec2 seg2End);

#endif
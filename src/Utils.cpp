#include "../include/Utils.h"
#include <algorithm>

using namespace std;

enum Coordinate {X, Y};

class CoordinateComparator {
public:
	CoordinateComparator(vec2* p, Coordinate coord) : points(p), coord(coord){}

	bool operator () (int i1, int i2){
		if (coord == X)
			return points[i1].x() < points[i2].x();
		else
			return points[i1].y() < points[i2].y();
	}

	vec2* points;
	Coordinate coord;
};

vector<int> PointSorter::byX(vec2 *points, int count){
	vector<int> resp;
	for (int i = 0; i < count; i++)
		resp.push_back(i);
	
	std::sort(resp.begin(), resp.end(), CoordinateComparator(points, X));
	return resp;
}

vector<int> PointSorter::byY(vec2 *points, int count){
	vector<int> resp;
	for (int i = 0; i < count; i++)
		resp.push_back(i);

	std::sort(resp.begin(), resp.end(), CoordinateComparator(points, Y));
	return resp;
}

class PolarComparator {
public:
	PolarComparator(vec2* p, int count, vec2 center){
		angles = new float[count];
		for (int i = 0; i < count; i++){
			if (p[i] == center)
				angles[i] = 0;
			else{
				vec2 diff = p[i] - center;
				float mag = diff.magnitude();
				angles[i] = acos(diff.x() / mag);
				if (diff.y() < 0)
					angles[i] = (2 * 3.14f) - angles[i];
			}
		}
	}

	bool operator () (int i1, int i2){
		return angles[i1] < angles[i2];
	}

	float* angles;
};

vector<int> PointSorter::byPolarAngle(vec2 *points, int count, vec2 center, int ignored){
	vector<int> resp;
	for (int i = 0; i < count; i++)
		if (i != ignored)
			resp.push_back(i);

	std::sort(resp.begin(), resp.end(), PolarComparator(points, count, center));
	return resp;
}
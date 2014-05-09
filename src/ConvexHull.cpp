#include "../include/GeneralProblems.h"

using namespace std;

class PointTuple {
public:
	PointTuple(){}
	PointTuple(const PointTuple & other){
		this->polarAngle = other.polarAngle;
		this->v = other.v;
	}

	PointTuple(vec2 v, vec2 center){
		this->v = v;

		vec2 diff = v - center;
		float mag = diff.magnitude();
		polarAngle = acos(diff.x() / mag);
		if (diff.y() < 0)
			polarAngle = -polarAngle;
	}
	float polarAngle;
	vec2 v;
};

vec2 getCenter(vec2 * points, int count){
	float sumX = 0, sumY = 0;
	for (int i = 0; i < count; i++){
		sumX += points[i].x();
		sumY += points[i].y();
	}
	return vec2(sumX / count, sumY / count);
}

static int comparePointTuple(const void *v1, const void *v2){
	PointTuple *p1 = (PointTuple*)v1,
		*p2 = (PointTuple*)v2;

	if (p1->polarAngle < p2->polarAngle)
		return -1;
	if (p1->polarAngle == p2->polarAngle)
		return 0;
	return 1;
}

static int getLeftmost(PointTuple *t, int count){
	int resp = 0;
	for (int i = 1; i < count; i++){
		if (t[i].v.x() < t[resp].v.x())
			resp = i;
	}
	return resp;
}

static bool smallAngle(vec2 v1, vec2 v2, vec2 center){
	float d1X = v1.x() - center.x(),
		d1Y = v1.y() - center.y(),
		d2X = v2.x() - center.x(),
		d2Y = v2.y() - center.y();
	return (d1X * d2Y - d2X * d1Y) >= 0;
}

static int oneLess(int n, int count){
	n--;
	if (n < 0)
		n += count;
	return n;
}


vector<vec2> getHull(vec2 * points, int count){
	vector<vec2> resp;

	vec2 center = getCenter(points, count);
	PointTuple *tuples = new PointTuple[count];
	for (int i = 0; i < count; i++){
		tuples[i] = PointTuple(points[i], center);
	}

	qsort(tuples, count, sizeof(PointTuple), comparePointTuple);
	int leftMost = getLeftmost(tuples, count);
	vec2 prev = tuples[leftMost].v;

	int last = oneLess(leftMost, count);
	int next = oneLess(last, count);
	resp.push_back(tuples[leftMost].v);


	while (next != last){
		int middle = (next + 1) % count;
		vec2 middleV = tuples[next].v;
		vec2 centerV = tuples[middle].v;
		if (smallAngle(prev, middleV, centerV)){
			resp.push_back(tuples[middle].v);
			prev = tuples[middle].v;

		}

		next = oneLess(next, count);
	}

	delete tuples;
	return resp;
}
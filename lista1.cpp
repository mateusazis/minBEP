#include "lista1.h"
#include "GeneralProblems.h"
#include <cmath>
#include <cstdio>
#include <cstring>


//Lista 1 - Exercícios de Primitivas Geométricas - Questão 4
Convexidade getConvexity(vec2 u, vec2 v, vec2 w){
	float uv = u.crossSign(v);
	if (uv == 0) //colineares
		return ERRO;
	
	if (uv < 0)
	{
		//reordená-los
		vec2 temp = v;
		v = u;
		u = temp;
		uv *= -1;
	}
	if (u.crossSign(w) == -uv || v.crossSign(w) == uv)
		return SIM; 
	return NAO;
}

//Lista 1 - Exercícios sobre Problemas Fundamentais - Questão 2
bool disjointsTriangles(vec2 tri1[3], vec2 tri2[3]){
	for (int i = 0; i < 3; i++){
		vec2 a = tri1[i], b = tri1[(i + 1) % 3];
		for (int j = 0; j < 3; j++){
			vec2 c = tri2[j], d = tri2[(j + 1) % 3];
			if (segmentIntersects(a, b, c, d))
				return false;
		}
	}
	return true;
}

class PointInfo{
public:
	PointInfo(){}
	PointInfo(vec2 *vec, int pos) : v(vec), originalPosition(pos){	}
	PointInfo(PointInfo &other) : v(other.v), originalPosition(other.originalPosition){	}

	float x(){
		return v->x();
	}
	float y(){
		return v->y();
	}

	float sqrDistance(PointInfo other){
		return v->sqrDistance(*(other.v));
	}

	static int compareX(const void *v1, const void *v2){
		PointInfo *a = (PointInfo*)v1;
		PointInfo *b = (PointInfo*)v2;
		if (a->x() < b->x())
			return -1;
		if (a->x() == b->x())
			return 0;
		return 1;
	}
	vec2 *v;
	int originalPosition;
};


int findNearPoints(PointInfo* allPoints, vec2 reference, int start, int end, int* dest, float maxDist, bool searchLeft){
	int count = 0;
	int i = searchLeft ? end : start;
	int offset = searchLeft ? -1 : 1;
	int last = searchLeft ? start - 1: end + 1;
	PointInfo candidate;
	//i += offset;
	while (i != last){
		candidate = allPoints[i];
		float distX = abs(candidate.x() - reference.x()),
			distY = abs(candidate.y() - reference.y());
		if ((distX <= maxDist) && (distY <= 2 * maxDist))
			dest[count++] = i;
		
		i += offset;
	}
	return count;
}

//Lista 1 - Exercícios sobre Problemas Fundamentais - Questão 3
static void _findClosestPair(PointInfo *points, int start, int end, int *index1, int *index2, int *aux1, int *aux2){
	int len = end - start + 1;
	if (len == 2)
	{
		*index1 = start;
		*index2 = end;
	}
	else if (len == 3){
		float dist1 = points[start].sqrDistance(points[start + 1]),
			dist2 = points[start].sqrDistance(points[end]),
			dist3 = points[start+1].sqrDistance(points[end]);
		if (dist1 < dist2 && dist1 < dist3){
			*index1 = start;
			*index2 = start + 1;
		}
		else if (dist2 < dist1 && dist2 < dist3){
			*index1 = start;
			*index2 = end;
		}
		else {
			*index1 = start + 1;
			*index2 = end;
		}
	}
	else {
		int middle = (start + end) / 2;
		int resp1i, resp1j, resp2i, resp2j;
		_findClosestPair(points, start, middle, &resp1i, &resp1j, aux1, aux2);
		_findClosestPair(points, middle + 1, end, &resp2i, &resp2j, aux1, aux2);
		float best1 = points[resp1i].sqrDistance(points[resp1j]);
		float best2 = points[resp2i].sqrDistance(points[resp2j]);
		int respi, respj;
		float best;
		if (best1 < best2)	{
			respi = resp1i; respj = resp1j; best = best1;
		}
		else {
			respi = resp2i; respj = resp2j; best = best2;
		}
		vec2 reference = *(points[middle].v);
		int sizeLeft =  findNearPoints(points, reference,      start, middle, aux1, best, true);
		int sizeRight = findNearPoints(points, reference, middle + 1,    end, aux2, best, false);

		for (int i = 0; i < sizeLeft; i++){
			PointInfo & pi = points[aux1[i]];
			for (int j = 0; j < sizeRight; j++){
				PointInfo & pj = points[aux2[j]];
				float newDist = pi.sqrDistance(pj);
				if (newDist < best){
					best = newDist;
					respi = aux1[i];
					respj = aux2[j];
				}
			}
		}
		*index1 = respi; *index2 = respj;
	}
}


//Lista 1 - Exercícios sobre Problemas Fundamentais - Questão 3
void findClosestPair(vec2 *points, int count, int *index1, int *index2){
	PointInfo* copy = new PointInfo[count];

	int *aux1 = new int[count],
		*aux2 = new int[count];
	for (int i = 0; i < count; i++)
		copy[i] = PointInfo(points+i, i);
	qsort(copy, count, sizeof(PointInfo), PointInfo::compareX);
	_findClosestPair(copy, 0, count - 1, index1, index2, aux1, aux2);
	//printf("i1 %d i2 %d\n", *index1, *index2);
	*index1 = copy[*index1].originalPosition;
	*index2 = copy[*index2].originalPosition;
	delete[] copy, aux1, aux2;
	/**index1 = 1;
	*index2 = 10;*/

}
#include "../include/lista1.h"
#include "../include/GeneralProblems.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;

//Lista 1 - Exercícios de Primitivas Geométricas - Questão 4
Convexidade getConvexity(vec2 u, vec2 v, vec2 w){
	int uv = u.crossSign(v);
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

//Lista 1 - Exercícios sobre Problemas Fundamentais - Questão 3

//Classe auxiliar, que armazena uma referência para um vetor e o índice do array ao qual ele pertencia.
class PointInfo{
public:
	PointInfo(){}
	PointInfo(vec2 *vec, int pos) : v(vec), index(pos){	}
	PointInfo(const PointInfo &other) : v(other.v), index(other.index){	}

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
	int index;
};

/*Procura pontos que estejam à esquerda ou à direita de reference, 
usando uma distância máxima de maxDist no eixo X e 2*maxDist no Y*/
static int findClosePoints(PointInfo* allPoints, vec2 reference, int start, int end, int* dest, float maxDist, bool searchLeft){
	int count = 0;
	int i = searchLeft ? end : start; //índice do primeiro elemento
	int increment = searchLeft ? -1 : 1;
	int last = searchLeft ? start - 1 : end + 1; //primeiro índice inválido após o último elemento
	PointInfo candidate;
	while (i != last){
		candidate = allPoints[i];
		float distX = abs(candidate.x() - reference.x()),
			distY = abs(candidate.y() - reference.y());
		if (distX <= maxDist && distY <= 2 * maxDist)
			dest[count++] = i;

		i += increment;
	}
	return count;
}

/*Retorna a menor distância entre 2 pontos de um subgrupo.
Coloca no índice de saída os índices desses pontos.*/
static float _findClosestPair(PointInfo *points, int start, int end, int *index1, int *index2, int *aux1, int *aux2){
	int len = end - start + 1;
	if (len == 1){ 
		/*No caso de um elemento, retorna distância infinita em vez de 0, 
		para que sempre seja pareado com algum ponto em outro grupo.*/
		*index1 = *index2 = start;
		return HUGE_VALF;
	}
	else {
		int middle = (start + end) / 2;
		int resp1i, resp1j, resp2i, resp2j;

		//Recursivamente, descobre os pares mais próximos nos grupos à esquerda e à direita
		float best1 = _findClosestPair(points, start, middle, &resp1i, &resp1j, aux1, aux2);
		float best2 = _findClosestPair(points, middle + 1, end, &resp2i, &resp2j, aux1, aux2);

		int respi, respj;
		float best;
		if (best1 < best2)	{
			respi = resp1i; respj = resp1j; best = best1;
		}
		else {
			respi = resp2i; respj = resp2j; best = best2;
		}

		//encontra os pontos à esquerda e à direita que são candidatos a estarem mais próximos
		vec2 reference = *(points[middle].v);
		int sizeLeft =  findClosePoints(points, reference,      start, middle, aux1, best, true);
		int sizeRight = findClosePoints(points, reference, middle + 1,    end, aux2, best, false);

		//verifica se algum deles está mais próximo
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
	return points[*index1].sqrDistance(points[*index2]);
}

pair<int, int> findClosestPair(vec2 *points, int count){
	//Vetores auxiliares, para guardar os índices dos pontos nos subgrupos.
	int *aux1 = new int[count],
		*aux2 = new int[count];

	//Cria e ordena uma cópia dos pontos, preservando os índices iniciais
	PointInfo* copy = new PointInfo[count];
	for (int i = 0; i < count; i++)
		copy[i] = PointInfo(points+i, i);
	qsort(copy, count, sizeof(PointInfo), PointInfo::compareX);
	
	int a, b; //índices no array ordenado
	_findClosestPair(copy, 0, count - 1, &a, &b, aux1, aux2);
	
	//Recupera os índices no array de entrada
	pair<int, int> resp(copy[a].index, copy[b].index);

	delete[] copy, aux1, aux2;

	return resp;	
}

//Lista 1 - Exercícios sobre Polígonos - Questão 2
int findLowestY(vec2* points, vector<int> & polygon){
	int resp = 0;
	float minY = points[polygon[0]].y();
	for (int i = 1; i < polygon.size(); i++){
		if (points[polygon[i]].y() < minY){
			minY = points[polygon[i]].y();
			resp = i;
		}
	}
	return resp;
}

static float det(float m[4]){
	return m[0] * m[3] - m[1] * m[2];
}

pair<float,float> basisChange(vec2 v, vec2 axis1, vec2 axis2){
	float mat[4] = {
		axis1.x(), axis2.x(),
		axis1.y(), axis2.y()
	};
	float delta = det(mat);
	mat[0] = v.x();
	mat[2] = v.y();
	float l1 = det(mat) / delta;
	mat[0] = axis1.x(), mat[2] = axis1.y();
	mat[1] = v.x(), mat[3] = v.y();
	float l2 = det(mat) / delta;
	return pair<float, float>(l1, l2);
}

void triangulate(vector<int> & resp, vec2* points, vector<int> polygon, int totalPointCount){
	int pointCount = polygon.size();
	if (pointCount <= 3){
		if (pointCount == 3){
			resp.push_back(polygon[0]);
			resp.push_back(polygon[1]);
			resp.push_back(polygon[2]);
		}
		return;
	}
		
	int lowestYIndex = findLowestY(points, polygon);
	int nextIndex = (lowestYIndex + 1) % pointCount;
	int previousIndex = lowestYIndex - 1;
	if (previousIndex < 0)
		previousIndex += pointCount;
	vec2 current = points[polygon[lowestYIndex]];
	vec2 prev = points[polygon[previousIndex]];
	vec2 next = points[polygon[nextIndex]];
	vec2 axis1 = prev - next;
	vec2 axis2 = mat3::rotation(3.14f / 2.0f) * axis1;
	float multPrev = basisChange(prev - current, axis1, axis2).second;
	float multNext = basisChange(next - current, axis1, axis2).second;
	float lowestMult = multPrev < multNext ? multPrev : multNext;

	int nextLowestVertexIndex = -1;

	for (int i = 0; i < polygon.size(); i++){
		if (i != lowestYIndex && i != nextIndex && i != previousIndex){
			vec2 v = points[polygon[i]];
			TriangleLocalization loc = findInTriangle(v, current, prev, next);
			if (loc == INSIDE){
				float mult = basisChange(v, axis1, axis2).second;
				if (mult < lowestMult){
					lowestMult = mult;
					nextLowestVertexIndex = i;
				}
			}
		}
	}

	if (nextLowestVertexIndex == -1){
		resp.push_back(polygon[lowestYIndex]);
		resp.push_back(polygon[nextIndex]);
		resp.push_back(polygon[previousIndex]);
		polygon.erase(polygon.begin() + lowestYIndex);
		triangulate(resp, points, polygon, totalPointCount);
	}
	else {
		vector<int> p1, p2;
		p1.push_back(polygon[lowestYIndex]);
		int index = (lowestYIndex + 1) % pointCount;
		while (index != nextLowestVertexIndex){
			p1.push_back(polygon[index]);
			index = (index + 1) % pointCount;
		}
		p1.push_back(polygon[nextLowestVertexIndex]);

		p2.push_back(polygon[lowestYIndex]);
		index = lowestYIndex - 1;
		if (index < 0)
			index += pointCount;
		while (index != nextLowestVertexIndex){
			p2.push_back(polygon[index]);
			index--;
			if (index < 0)
				index += pointCount;
		}
		p2.push_back(polygon[nextLowestVertexIndex]);
		triangulate(resp, points, p1, totalPointCount);
		triangulate(resp, points, p2, totalPointCount);
	}
}

vector<int> divideAndConquerTriangulate(vec2 *points, int count){
	vector<int> resp;

	vector<int> indices;
	for (int i = 0; i < count; i++)
		indices.push_back(i);

	triangulate(resp, points, indices, count);

	return resp;
}

//Lista 1 - Exercícios sobre Polígonos - Questão 1
//Nota: Pontos devem ser dados no sentido horário
bool convex(vec2* polygon, int pointCount){
	if (pointCount <= 3)
		return true;
	
	vec2 diff1 = polygon[1] - polygon[0];
	vec2 diff2 = polygon[2] - polygon[0];
	int firstSign = diff1.crossSign(diff2);

	for (int i = 0; i < pointCount; i++){
		int current = (i + 1) % pointCount;
		int next = (i + 2) % pointCount;
		diff1 = polygon[current] - polygon[i];
		diff2 = polygon[next] - polygon[i];
		if (diff1.crossSign(diff2) != firstSign)
			return false;
	}
	return true;
}

//Lista 1 - Exercícios sobre Polígonos - Questão 4
static bool isCenterOfEar(vector<PointInfo> v, int i){
	int size = v.size();
	int previous = i - 1;
	if (previous < 0)
		previous += size;
	int next = (i + 1) % size;

	vec2 vPrevious = *v[previous].v,
		vI = *v[i].v,
		vNext = *v[next].v;

	if ((vPrevious - vI).crossSign(vNext - vI) < 0)
		return false;

	for (int j = (next + 1) % size; j != previous; j = (j + 1) % size){
		TriangleLocalization l = findInTriangle(*v[j].v, vPrevious, vI, vNext);
		if (l == INSIDE)
			return false;
	}
	return true;
}

/*
Nota: vértices do polígono devem estar no sentido horário!
*/
vector<int> earClippingTriangulate(vec2 *points, int count){
	vector<int> resp;

	//Cria uma cópia dos vértices, salvando os índices originais
	vector<PointInfo> vertices;
	for (int i = 0; i < count; i++)
		vertices.push_back(PointInfo(points + i, i));

	for (int i = 0; i < vertices.size(); i++){
		if (isCenterOfEar(vertices, i)){
			//Sempre que um vértice, o anterior e o posterior formarem uma orelha, remover esse vértice do polígono.
			int previous = i - 1;
			if (previous < 0)
				previous += vertices.size();
			int next = (i + 1) % vertices.size();
			resp.push_back(vertices[previous].index);
			resp.push_back(vertices[i].index);
			resp.push_back(vertices[next].index);
			vertices.erase(vertices.begin() + i);
			i--;
		}
	}

	return resp;
}



















class PointTuple2 {
public:
	PointTuple2(){}
	PointTuple2(const PointTuple2 & other){
		this->polarAngle = other.polarAngle;
		this->v = other.v;
		this->index = other.index;
	}

	PointTuple2(vec2 v, vec2 center, int index) : index(index){
		this->v = v;

		vec2 diff = v - center;
		float mag = diff.magnitude();
		polarAngle = acos(diff.x() / mag);
		if (diff.y() < 0)
			polarAngle = -polarAngle;
	}
	float polarAngle;
	vec2 v;
	int index;
};

static int comparePointTuple2(const void *v1, const void *v2){
	PointTuple2 *p1 = (PointTuple2*)v1,
		*p2 = (PointTuple2*)v2;

	if (p1->polarAngle < p2->polarAngle)
		return -1;
	if (p1->polarAngle == p2->polarAngle)
		return 0;
	return 1;
}

static float _getPointDistance(vec2 v1, vec2 v2){
	return sqrt((v1 - v2).magnitude());
}
vec2 getCenter(vec2 * points, int count);
static int getCenterMostVertex(vec2 * points, int count){
	vec2 center = getCenter(points, count);
	int resp = 0;
	float dist = _getPointDistance(points[0], center);
	for (int i = 1; i < count; i++){
		float newDist = _getPointDistance(points[i], center);
		if (newDist < dist)
		{
			resp = i;
			dist = newDist;
		}
	}
	return resp;
}

vector<int> incrementalTriangulate(vec2 *points, int count){
	vector<int> resp;

	if (count == 3){
		resp.push_back(0);
		resp.push_back(1);
		resp.push_back(2);
	}
	else if (count > 3){
		int centerMost = getCenterMostVertex(points, count);
		PointTuple2 *tps = new PointTuple2[count - 1];

		int n = 0;
		for (int i = 0; i < count; i++){
			if (i != centerMost)
				tps[n++] = PointTuple2(points[i], points[centerMost], i);
		}

		qsort(tps, count - 1, sizeof(PointTuple2), comparePointTuple2);
		
		for (int i = 0; i < count - 1; i++){
			int j = (i + 1) % (count - 1);
			resp.push_back(tps[i].index);
			resp.push_back(centerMost);
			resp.push_back(tps[j].index);
		}

		for (int i = 0; i < count - 1; i++){
			int j = (i + 1) % (count - 1);
			int k = (i + 2) % (count - 1);

			vec2 prev = tps[i].v;
			vec2 curr = tps[j].v;
			vec2 next = tps[k].v;
			if ((prev - curr).crossSign(next - curr) > 0){
				resp.push_back(tps[i].index);
				resp.push_back(tps[j].index);
				resp.push_back(tps[k].index);
			}
		}

		delete[] tps;
	}

	return resp;
}
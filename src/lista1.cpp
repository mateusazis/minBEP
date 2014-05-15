#include "../include/lista1.h"
#include "../include/GeneralProblems.h"
#include "../include/Utils.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <deque>

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

/*Retorna a quatnidade de pontos que estejam à esquerda ou à direita de reference, 
usando uma distância máxima de maxDist no eixo X e 2*maxDist no Y*/
static int findClosePoints(const int* indices, vec2 reference, int start, int end, int* dest, float maxDist, bool searchLeft, vec2* pts){
	int count = 0;
	int i = searchLeft ? end : start; //índice do primeiro elemento
	int increment = searchLeft ? -1 : 1;
	int last = searchLeft ? start - 1 : end + 1; //primeiro índice inválido após o último elemento
	vec2 candidate;
	while (i != last){
		candidate = pts[indices[i]];
		float distX = abs(candidate.x() - reference.x()),
			distY = abs(candidate.y() - reference.y());
		if (maxDist == HUGE_VALF || (distX <= maxDist && distY <= 2 * maxDist))
			dest[count++] = i;

		i += increment;
	}
	return count;
}

/*Retorna a menor distância entre 2 pontos de um subgrupo.
Coloca no índice de saída os índices desses pontos.*/
static float _findClosestPair(const int *indices, int start, int end, pair<int, int> & resp, int *aux1, int *aux2, vec2* pts){
	int len = end - start + 1;
	if (len == 1){ 
		/*No caso de um elemento, retorna distância infinita em vez de 0, 
		para que sempre seja pareado com algum ponto em outro grupo.*/
		resp = pair<int, int>(start, start);
		return HUGE_VALF;
	}
	else {
		int middle = (start + end) / 2;
		pair<int, int> resp1, resp2;

		//Recursivamente, descobre os pares mais próximos nos grupos à esquerda e à direita
		float best1 = _findClosestPair(indices, start, middle, resp1, aux1, aux2, pts);
		float best2 = _findClosestPair(indices, middle + 1, end, resp2, aux1, aux2, pts);

		float best;
		if (best1 < best2)	{
			resp = resp1;
			best = best1;
		}
		else {
			resp = resp2;
			best = best2;
		}

		//encontra os pontos à esquerda e à direita que são candidatos a estarem mais próximos
		vec2 reference = pts[indices[middle]];
		int sizeLeft  = findClosePoints(indices, reference,   start, middle, aux1, best,  true, pts);
		int sizeRight = findClosePoints(indices, reference, middle + 1, end, aux2, best, false, pts);

		//verifica se algum deles está mais próximo
		for (int i = 0; i < sizeLeft; i++){
			int pi = indices[aux1[i]];
			for (int j = 0; j < sizeRight; j++){
				int pj = indices[aux2[j]];
				float newDist = (pts[pi] - pts[pj]).magnitude();
				if (newDist < best){
					best = newDist;
					resp = pair<int, int>(pi, pj);
				}
			}
		}
	}
	return pts[resp.first].sqrDistance(pts[resp.second]);
}

pair<int, int> findClosestPair(vec2 *points, int count){
	//Vetores auxiliares, para guardar os índices dos pontos nos subgrupos.
	int *aux1 = new int[count],
		*aux2 = new int[count];

	//Cria e ordena uma cópia dos pontos, preservando os índices iniciais
	vector<int> sortedIndices = PointSorter::byX(points, count);
	
	pair<int, int> resp;
	_findClosestPair(sortedIndices.data(), 0, count - 1, resp, aux1, aux2, points);

	delete[] aux1, aux2;

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
static bool isCenterOfEar(vector<int> v, int i, vec2* points){
	int size = v.size();
	int previous = i - 1;
	if (previous < 0)
		previous += size;
	int next = (i + 1) % size;

	vec2 vPrevious = points[v[previous]],
		vI = points[v[i]],
		vNext = points[v[next]];

	if ((vPrevious - vI).crossSign(vNext - vI) < 0)
		return false;

	for (int j = (next + 1) % size; j != previous; j = (j + 1) % size){
		TriangleLocalization l = findInTriangle(points[v[j]], vPrevious, vI, vNext);
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
	vector<int> vertices;
	for (int i = 0; i < count; i++)
		vertices.push_back(i);

	for (int i = 0; i < vertices.size() && vertices.size() >= 3; i++){
		if (isCenterOfEar(vertices, i, points)){
			//Sempre que um vértice, o anterior e o posterior formarem uma orelha, remover esse vértice do polígono.
			int previous = i - 1;
			if (previous < 0)
				previous += vertices.size();
			int next = (i + 1) % vertices.size();
			resp.push_back(vertices[previous]);
			resp.push_back(vertices[i]);
			resp.push_back(vertices[next]);
			vertices.erase(vertices.begin() + i);
			i--;
		}
	}

	return resp;
}

static void _findVisibleVertices(deque<int> & hull, vec2* points, vec2 newPoint, int & outTopMost, int & outBottomMost){
	outTopMost = 0;
	outBottomMost = 0;
	float angleTopMost = (points[hull[0]] - newPoint).orientedAngle();
	float angleBottomMost = angleTopMost;
	for (int j = 1; j < hull.size(); j++){
		vec2 v = points[hull[j]];
		float angle = (v - newPoint).orientedAngle();
		if (angle > angleBottomMost){
			angleBottomMost = angle;
			outBottomMost = j;
		}
		if (angle < angleTopMost){
			angleTopMost = angle;
			outTopMost = j;
		}
	}
}

vector<int> incrementalTriangulate(vec2 *points, int count){
	vector<int> resp; //índices dos vértices dos triângulos

	if (count >= 3){
		vector<int> indices = PointSorter::byX(points, count); //índices dos pontos ordenados no eixo X
		resp.push_back(indices[0]);
		resp.push_back(indices[1]);
		resp.push_back(indices[2]);

		deque<int> hull; //guardará, em sentido anti-horário, os índices dos vértices do fecho convexo
		/* Nota: usei deque pois há muitas remoções, o que,
		   segundo http://www.cplusplus.com/reference/vector/vector/erase/ ,
		   é ineficiente em vectors. O uso de listas também é inadequado por não serem facilmente indexáveis.*/

		hull.push_back(indices[0]);
		if ((points[indices[1]] - points[indices[2]]).crossMag(points[indices[0]] - points[indices[1]]) > 0){
			hull.push_back(indices[1]);
			hull.push_back(indices[2]);
		}
		else {
			hull.push_back(indices[2]);
			hull.push_back(indices[1]);
		}

		for (int i = 3; i < indices.size(); i++){
			vec2 newPoint = points[indices[i]];

			int topMost = 0;    //índice, no fecho, do vértice "mais anterior", ou seja, com menor ângulo
			int bottomMost = 0; //índice, no fecho, do vértice "mais posterior", ou seja, com maior ângulo
			_findVisibleVertices(hull, points, newPoint, topMost, bottomMost);

			int k = topMost;
			while (k != bottomMost){ //insere triângulos envolvendo os vértices adjacentes do fecho e o novo vértice
				int m = (k + 1) % hull.size();
				resp.push_back(hull[k]);
				resp.push_back(indices[i]);
				resp.push_back(hull[m]);
				k = (k + 1) % hull.size();
			}

			//apaga do fecho os pontos do trecho visível, exceto pelos extremos
			if (bottomMost == 0)
				hull.erase(hull.begin() + topMost + 1, hull.end());
			else
				hull.erase(hull.begin() + topMost + 1, hull.begin() + bottomMost);
			
			//insere o ponto atual onde o trecho apagado estava, substituindo-o
			hull.insert(hull.begin() + topMost + 1, indices[i]);
		}
	}
	return resp;
}

//Questão 5
void getAspectRatio(vec2* points, vector<int> &triangles, float &minRatio, float &maxRatio, float &average){
	minRatio = HUGE_VALF;
	maxRatio = 0;
	float ratioSum = 0;
	for (int i = 0; i < triangles.size(); i+=3){
		float d1 = points[triangles[i]].distance(points[triangles[i + 1]]),
			d2 = points[triangles[i]].distance(points[triangles[i + 2]]),
			d3 = points[triangles[i+1]].distance(points[triangles[i + 2]]);
		float minD = std::min(d1, std::min(d2, d3));
		float maxD = std::max(d1, std::max(d2, d3));
		float ratio = maxD / minD;
		minRatio = std::min(minRatio, ratio);
		maxRatio = std::max(maxRatio, ratio);
		ratioSum += ratio;
	}
	average = ratioSum / (triangles.size() / 3);
}

void getSkewness(vec2* points, vector<int> &triangles, float &min, float &max, float &avg){
	min = HUGE_VALF;
	max = 0;
	float sum = 0;
	for (int i = 0; i < triangles.size(); i += 3){
		vec2 p1 = points[triangles[i]],
			p2 = points[triangles[i + 1]],
			p3 = points[triangles[i + 2]];
		float aT = triangleArea(p1, p2, p3);

		float diameter = (p1.distance(p2) * p1.distance(p3) * p2.distance(p3)) / (2.0f * aT);
		float r = diameter / 2.0f;
		float aCT = (3.0f * r * r * sqrt(3)) / 4.0f;
		float sk = aCT - aT;


		min = std::min(min, sk);
		max = std::max(max, sk);
		sum += sk;
	}
	avg = sum / (triangles.size() / 3);
}

void getSmoothness(vec2* points, vector<int> &triangles, float &min, float &max, float &avg){
	if (triangles.size() <= 3){
		min = max = avg = 0;
		return;
	}
	min = HUGE_VALF;
	max = 0;
	float sum = 0;
	int count = 0;

	for (int i = 0; i < triangles.size() - 3; i += 3){
		vec2 p1 = points[triangles[i]],
			p2 = points[triangles[i + 1]],
			p3 = points[triangles[i + 2]];
		float a1 = triangleArea(p1, p2, p3);

		for (int j = i + 3; j < triangles.size(); j+=3){
			int sharedVertices = 0;
			vector<int>::iterator last = triangles.begin() + j + 3;
			sharedVertices += std::find(triangles.begin() + j, last, triangles[i]) != last;
			sharedVertices += std::find(triangles.begin() + j, last, triangles[i+1]) != last;
			sharedVertices += std::find(triangles.begin() + j, last, triangles[i+2]) != last;
			bool adjacents = sharedVertices > 1;
			if (adjacents){
				count++;
				float a2 = triangleArea(points[triangles[j]], points[triangles[j+1]], points[triangles[j+2]]);
				float diff = abs(a1 - a2);
				min = std::min(min, diff);
				max = std::max(max, diff);
				sum += diff;
			}
		}
	}
	//delete [] checked;
	avg = sum / count;
}
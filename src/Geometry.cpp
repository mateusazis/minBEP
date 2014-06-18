#include "../include/Geometry.h"
#include <deque>
#include <algorithm>

using namespace std;

static float det(float m[9]){
	return m[0] * m[4] * m[8] +
		m[1] * m[5] * m[6] +
		m[2] * m[3] * m[7] -
		m[2] * m[4] * m[6] -
		m[5] * m[7] * m[0] -
		m[1] * m[3] * m[8];
}

static unsigned int getSign(float f){
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

//Lista 1 - Exercícios sobre Polígonos - Questão 4
static bool isCenterOfEar(const deque<int> & v, int i, const vec2* points, bool clockwise){
	int size = v.size();
	int previous = (i + size - 1) % size;;
	int next = (i + 1) % size;

	vec2 vPrevious = points[v[previous]],
		vI = points[v[i]],
		vNext = points[v[next]];

	int expectedSign = clockwise ? 1 : -1;

	if ((vPrevious - vI).crossSign(vNext - vI) == expectedSign)
		return false;
	

	for (int j = (next + 1) % size; j != previous; j = (j + 1) % size){
		TriangleLocalization l = findInTriangle(points[v[j]], vPrevious, vI, vNext);
		if (l == INSIDE)
			return false;
	}

	return true;
}

static bool isPolygonClockwise(const vec2* points, int count){
	if (count >= 3){
		vec2 v1 = points[1] - points[0], v2 = points[2] - points[0];
		return v1.crossSign(v2) > 0;
	}
	return false; //less than 3 points, no orientation!
}

/*
Nota: vértices do polígono devem estar no sentido horário!
*/
vector<int> earClippingTriangulate(const vec2 *points, int count){
	vector<int> resp;
	int polygonCount = std::max(0, count - 2);

	//Cria uma cópia dos vértices, salvando os índices originais
	deque<int> vertices;
	for (int i = 0; i < count; i++)
		vertices.push_back(i);

	bool clockwise = isPolygonClockwise(points, count);

	bool foundAny;

	while (resp.size() < polygonCount * 3){
		foundAny = false;
		for (int i = 0; i < vertices.size(); i++){
			if (isCenterOfEar(vertices, i, points, clockwise)){
				//Sempre que um vértice, o anterior e o posterior formarem uma orelha, remover esse vértice do polígono.
				int previous = (i + vertices.size() - 1) % vertices.size();
				int next = (i + 1) % vertices.size();
				resp.push_back(vertices[previous]);
				resp.push_back(vertices[i]);
				resp.push_back(vertices[next]);
				vertices.erase(vertices.begin() + i);
				//i--;
				foundAny = true;
				break;
			}
		}
		if (!foundAny){ //emergency exit
			resp.clear();
			return resp;
		}
	}

	return resp;
}

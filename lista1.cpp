#include "lista1.h"
#include "GeneralProblems.h"
#include <cmath>
#include <cstdio>
#include <cstring>

using namespace std;

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

//Lista 1 - Exercícios sobre Problemas Fundamentais - Questão 3

//Classe auxiliar, que armazena uma referência para um vetor e o índice do array ao qual ele pertencia.
class PointInfo{
public:
	PointInfo(){}
	PointInfo(vec2 *vec, int pos) : v(vec), index(pos){	}
	PointInfo(PointInfo &other) : v(other.v), index(other.index){	}

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

std::pair<int, int> findClosestPair(vec2 *points, int count){
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
#ifndef LISTA_1_H
#define LISTA_1_H

#include "../include/angel.h"
#include <utility>
#include <vector>

enum Convexidade {ERRO, SIM, NAO};

//Lista 1

/*Lista 1 - Exerc�cios de Primitivas Geom�tricas - Quest�o 4
U: vetor 1
V: vetor 2
W: vetor que deve estar no meio de U e V
*/
Convexidade getConvexity(vec2 u, vec2 v, vec2 w);




//Exerc�cios sobre Pol�gonos
//Quest�o 1
bool convex(vec2* polygon, int pointCount);

//Quest�o 2
std::vector<int> divideAndConquerTriangulate(const vec2 *points, int count);

//Quest�o 4
std::vector<int> earClippingTriangulate(vec2 *points, int count);




//Exerc�cios sobre Problemas Fundamentais
//Quest�o 2
bool disjointsTriangles(vec2 tri1[3], vec2 tri2[3]);

//Quest�o 3
std::pair<int, int> findClosestPair(vec2 *points, int count);

//Quest�o 4
std::vector<int> incrementalTriangulate(vec2 *points, int count);

//Quest�o 5
void getAspectRatio(vec2* points, std::vector<int> &triangles, float &min, float &max, float &average);
void getSkewness(vec2* points, std::vector<int> &triangles, float &min, float &max, float &average);
void getSmoothness(vec2* points, std::vector<int> &triangles, float &min, float &max, float &average);

#endif
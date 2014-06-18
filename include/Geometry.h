#ifndef GC_PROBLEMS_H
#define GC_PROBLEMS_H

#include "angel.h"

#include <vector>

enum TriangleLocalization { INSIDE = 7, P1_AREA = 4, P2_AREA = 2, P3_AREA = 1, P1P2_AREA = 6, P2P3_AREA = 3, P1P3_AREA = 5};
TriangleLocalization findInTriangle(vec2 point, vec2 p1, vec2 p2, vec2 p3);

//Lista 1
enum Convexidade { ERRO, SIM, NAO };
/*Lista 1 - Exercícios de Primitivas Geométricas - Questão 4
U: vetor 1
V: vetor 2
W: vetor que deve estar no meio de U e V
*/
Convexidade getConvexity(vec2 u, vec2 v, vec2 w);

//Questão 2
std::vector<int> divideAndConquerTriangulate(const vec2 *points, int count);

//Questão 4
std::vector<int> earClippingTriangulate(const vec2 *points, int count);

#endif
#ifndef LISTA_1_H
#define LISTA_1_H

#include "../include/angel.h"
#include <utility>
#include <vector>

enum Convexidade {ERRO, SIM, NAO};

//Lista 1 - Exercícios de Primitivas Geométricas - Questão 4
Convexidade getConvexity(vec2 u, vec2 v, vec2 w);

//Lista 1 - Exercícios sobre Problemas Fundamentais - Questão 2
bool disjointsTriangles(vec2 tri1[3], vec2 tri2[3]);

//Lista 1 - Exercícios sobre Problemas Fundamentais - Questão 3
std::pair<int, int> findClosestPair(vec2 *points, int count);


//Lista 1 - Exercícios sobre Polígonos - Questão 1
bool convex(vec2* polygon, int pointCount);

//Lista 1 - Exercícios sobre Polígonos - Questão 2
std::vector<int> divideAndConquerTriangulate(vec2 *points, int count);

//Lista 1 - Exercícios sobre Polígonos - Questão 4
std::vector<int> earClippingTriangulate(vec2 *points, int count);

#endif
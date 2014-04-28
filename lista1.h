#ifndef LISTA_1_H
#define LISTA_1_H

#include "angel.h"

enum Convexidade {ERRO, SIM, NAO};

Convexidade getConvexity(vec2 u, vec2 v, vec2 w);

bool disjointsTriangles(vec2 tri1[3], vec2 tri2[3]);

#endif
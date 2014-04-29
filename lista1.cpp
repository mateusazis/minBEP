#include "lista1.h"
#include "GeneralProblems.h"
#include <cmath>
#include <cstdio>


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
#include "lista1.h"
#include <cmath>
#include <cstdio>


//Lista 1 - Exerc�cios de Primitivas Geom�tricas - Quest�o 4
Convexidade getConvexity(vec2 u, vec2 v, vec2 w){
	float uv = u.crossSign(v);
	if (uv == 0) //colineares
		return ERRO;
	
	if (uv < 0)
	{
		//reorden�-los
		vec2 temp = v;
		v = u;
		u = temp;
		uv *= -1;
	}
	if (u.crossSign(w) == -uv || v.crossSign(w) == uv)
		return SIM; 
	return NAO;
}

//Lista 1 - Exerc�cios sobre Problemas Fundamentais - Quest�o 2
#ifndef ANGEL_H
#define ANGEL_H

class vec2{
public:
	vec2();
	vec2(const vec2 & other);
	vec2(float x, float y);

	float magnitude();
	float & x();
	float & y();

	const float* data();

	int crossSign(vec2 other);

private:
	float v[2];
};

vec2 operator +(vec2 v, vec2 other);
vec2 operator -(vec2 v, vec2 other);
vec2 operator *(vec2 v, float value);
vec2 operator /(vec2 v, float value);

class mat3{
public:
	mat3();
	mat3(const mat3 & other);

	static mat3 identity();
	static mat3 rotation(float angle);

	float & at(int row, int column);

	const float* data();
private:
	float v[3][3];
};

vec2 operator *(mat3 m, vec2 v);

float random();

#endif
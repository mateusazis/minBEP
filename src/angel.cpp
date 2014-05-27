#include "../include/angel.h"
#include <cmath>
#include <cstring>
#include <cstdlib>

vec2::vec2() {
	this->v[0] = 0;
	this->v[1] = 0;
}

vec2::vec2(const vec2 & other){
	this->v[0] = other.v[0];
	this->v[1] = other.v[1];
}

vec2::vec2(float x, float y) {
	v[0] = x;
	v[1] = y;
}

float & vec2::x(){
	return v[0];
}

float & vec2::y(){
	return v[1];
}

const float* vec2::data(){
	return v;
}

float vec2::dot(vec2 other){
	return x() * other.x() + y() * other.y();
}

float vec2::crossMag(vec2 other){
	return x() * other.y() - y() * other.x();
}

int vec2::crossSign(vec2 other){
	float cross = crossMag(other);
	if (cross > 0)
		return 1;
	if (cross == 0)
		return 0;
	return -1;
}

float vec2::magnitude(){
	return sqrt(v[0] * v[0] + v[1] * v[1]);
}

float vec2::sqrDistance(vec2 other){
	float diffX = other.x() - x(),
		diffY = other.y() - y();
	return diffX * diffX + diffY * diffY;
}

float vec2::distance(vec2 other){
	return sqrt(sqrDistance(other));
}

float vec2::orientedAngle(){
	float resp = acos(x() / magnitude());
	if (y() < 0)
		resp = (2 * 3.14f) - resp;
	return resp;
}

vec2 vec2::normalized(){
	return (*this) / this->magnitude();
}

bool operator ==(vec2 v, vec2 other){
	return v.x() == other.x() && v.y() == other.y();
}

vec2 operator +(vec2 v, vec2 other){
	return vec2(v.x() + other.x(), v.y() + other.y());
}

vec2 operator -(vec2 v, vec2 other){
	return vec2(v.x() - other.x(), v.y() - other.y());
}

vec2 operator *(vec2 v, float value){
	return vec2(v.x() * value, v.y() * value);
}

vec2 operator /(vec2 v, float value){
	return vec2(v.x() / value, v.y() / value);
}

mat3::mat3(){
	memset(v, 0, 3 * 3 * sizeof(float));
}

mat3::mat3(const mat3 & other){
	memcpy(v, other.v, 3 * 3 * sizeof(float));
}

float & mat3::at(int row, int column){
	return v[row][column];
}

mat3 mat3::identity(){
	mat3 resp;

	resp.at(0, 0) = 1;
	resp.at(1, 1) = 1;
	resp.at(2, 2) = 1;

	return resp;
}

mat3 mat3::rotation(float angle){
	mat3 resp = identity();

	float c = cos(angle);
	float s = sin(angle);

	resp.at(0, 0) = c; resp.at(0, 1) = -s;
	resp.at(1, 0) = s; resp.at(1, 1) = c;

	return resp;
}

mat3 mat3::translation(float x, float y){
	mat3 resp = identity();

	resp.at(0, 2) = x;
	resp.at(1, 2) = y;

	return resp;
}

const float* mat3::data(){
	return &(v[0][0]);
}

vec2 operator *(mat3 m, vec2 v){
	float x = m.at(0, 0) * v.x() + m.at(0, 1) * v.y() + m.at(0, 2) * 1; //w hardcoded as 1
	float y = m.at(1, 0) * v.x() + m.at(1, 1) * v.y() + m.at(1, 2) * 1;
	return vec2(x, y);
}


float random(){
	const int MAX = 1000;
	int newR = rand() % MAX;
	return newR / (MAX - 1.0f);
}

float random(float min, float max){
	return min + (max - min) * random();
}
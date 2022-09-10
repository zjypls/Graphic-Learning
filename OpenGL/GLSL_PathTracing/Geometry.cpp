//
// Created by 32725 on 2022/8/16.
//

#include "Geometry.h"

vec3 operator*(const vec3&a ,const vec3& b){return {a.y()*b.z()-a.z()*b.y(),-a.x()*b.z()+a.z()* b.x(),a.x()*b.y()-a.y()*b.x()};};
vec3 operator*(const vec3&a,float b){return {a.x()*b,a.y()*b,a.z()*b};}
vec3 operator+(const vec3&a,const vec3& b){return {a.x()+b.x(),a.y()+b.y(),a.z()+b.z()};}
vec3 operator-(const vec3&a,const vec3& b){return {a.x()-b.x(),a.y()-b.y(),a.z()-b.z()};}
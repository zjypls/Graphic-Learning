//
// Created by 32725 on 2022/8/16.
//

#ifndef GLSL_PATHTRACING_GEOMETRY_H
#define GLSL_PATHTRACING_GEOMETRY_H
#define LAMBERT 0
#define MIRR 1
//#define METAL 2
#define LIGHT 2
#define GLASS 3
#include <cmath>
#include <iostream>
#include <algorithm>
struct vec3 {
    float pos[3];
    vec3():pos{0,0,0}{}
    vec3(float i,float j,float k):pos{i,j,k}{}
    vec3(float i):pos{i,i,i}{}
    inline float length()const{
        return sqrtf(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]);
    }
    inline vec3 operator/(float a)const{
        return {x()/a,y()/a,z()/a};
    }
    inline vec3 normalize()const{
        return *this/this->length();
    }
    inline const float *const ptr() const{
        return pos;
    }
    inline float x()const{
        return pos[0];
    }
    inline float y()const{
        return pos[1];
    }
    inline float z()const{
        return pos[2];
    }
};

struct Sphere {
    vec3 center;
    float radius;
    int type;
    vec3 color;
};

struct Face{
    float xmin,xmax,ymin,ymax,zmin,zmax;
    int type;
    vec3 Normal;
    vec3 Point;
    vec3 color;
};
vec3 operator*(const vec3&a ,const vec3& b);
vec3 operator*(const vec3&a,float b);
vec3 operator+(const vec3&a,const vec3& b);
vec3 operator-(const vec3&a,const vec3& b);
struct Camera{
    vec3 origin;
    vec3 horizon;
    vec3 vertical;
    vec3 leftcorner;
    vec3 color;
    vec3 ForWord;
    float theY,theZ,focus,lmd;
    void Update(double dz,double dy){
        theY+=dy*lmd;
        theZ+=dz*lmd;
        if(theY<0.001)
            theY=0.001;
        else if(theY>3.12)
            theY=3.12;
        if(theZ< -1.55)
            theZ=-1.55;
        else if(theZ>1.55)
            theZ=1.55;
        ForWord=vec3{sinf(theY)*sinf(theZ),static_cast<float>(cos(theY)), -sinf(theY)*cosf(theZ)}.normalize();
        horizon=(ForWord*vec3(0,1,0)).normalize()*horizon.length();
        vertical=(horizon*ForWord).normalize()*vertical.length();
        leftcorner=(origin+(ForWord*focus)-horizon*0.5f-vertical*0.5f);
    }
};

#endif //GLSL_PATHTRACING_GEOMETRY_H

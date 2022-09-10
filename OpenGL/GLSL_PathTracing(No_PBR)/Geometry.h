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

struct vec3 {
    float pos[3];
    vec3():pos{0,0,0}{}
    vec3(float i,float j,float k):pos{i,j,k}{}
    vec3(float i):pos{i,i,i}{}
    inline const float *const ptr() const{
        return pos;
    }
    inline float x(){
        return pos[0];
    }
    inline float y(){
        return pos[1];
    }
    inline float z(){
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

struct Camera{
    vec3 origin;
    vec3 horizon;
    vec3 vertical;
    vec3 leftcorner;
    vec3 color;
};

#endif //GLSL_PATHTRACING_GEOMETRY_H

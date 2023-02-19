#pragma once
#include<glad/glad.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include "Dep/Shader.h"
#include "Geometry.h"
using namespace std;


class RayShader: public Shader{
public:
    RayShader()=default;

    void Set(const char*,float);
    void Set(const char*,int);
    void Set(const char*,const vec3&);
    void AddObject(const Sphere&,const int);
    void AddObject(const Face&,const int);
};



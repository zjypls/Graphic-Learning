#pragma once
#include<glad/glad.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include "Geometry.h"
using namespace std;


class Shader {
public:
	Shader(const string&, const string&, const string geom = "");
    Shader(){}
	inline void use() {
		glUseProgram(Program);
	}

    void Set(const char*,float);
    void Set(const char*,int);
    void Set(const char*,const vec3&);
    void AddObject(const Sphere&,const int);
    void AddObject(const Face&,const int);

	unsigned Program;
};



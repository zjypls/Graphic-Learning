#include "RayShader.h"

void RayShader::Set(const char *name, float value) {
    glUniform1f(glGetUniformLocation(Program, name), value);
}

void RayShader::Set(const char *name, int value) {
    glUniform1i(glGetUniformLocation(Program, name), value);
}

void RayShader::Set(const char *name, const vec3 &value) {
    glUniform3fv(glGetUniformLocation(Program, name), 1, value.ptr());
}

void RayShader::AddObject(const Sphere &sphere, const int index) {
//    char name[18]="world.spheres[0].";
//    sprintf(name,"world.spheres[%d].",index);
    string name = string("world.spheres[") + to_string(index) + "].";
    Set((name + "center").c_str(), sphere.center);
    Set((name + "radius").c_str(), sphere.radius);
    Set((name + "type").c_str(), sphere.type);
    Set((name + "color").c_str(), sphere.color);
}

void RayShader::AddObject(const Face &face, const int index) {
    string name = string("world.faces[") + to_string(index) + "].";
    Set((name + "xmax").c_str(), face.xmax);
    Set((name + "ymax").c_str(), face.ymax);
    Set((name + "zmax").c_str(), face.zmax);
    Set((name + "xmin").c_str(), face.xmin);
    Set((name + "ymin").c_str(), face.ymin);
    Set((name + "zmin").c_str(), face.zmin);
    Set((name + "type").c_str(), face.type);
    Set((name + "Normal").c_str(), face.Normal);
    Set((name + "Point").c_str(), face.Point);
    Set((name + "color").c_str(), face.color);

}


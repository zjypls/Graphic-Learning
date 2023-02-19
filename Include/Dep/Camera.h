//
// Created by 32725 on 2022/10/20.
//

#ifndef CHAP01_CAMERA_H
#define CHAP01_CAMERA_H
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "../GLFW/include/glfw/glfw3.h"
using glm::vec3;

class Camera {
public:
    vec3 origin,lookat,up;
    float thY,thX,width,height,lmd,fov;
    double lsX,lsY;
    bool WalkMode,ScreenShot,Resize,MoveMode;
    glm::vec2 scrSize,scrOffset;
    vec3 scrHorizon,scrVertical;
    glm::mat4 GetTransform(const vec3&offset={})const;
    void walking(GLFWwindow*);

    static void Turn(GLFWwindow*,double,double);
    static void Walk(GLFWwindow*);
    static void ResizeCall(GLFWwindow*,int,int);
    static void Scroll(GLFWwindow*,double,double);

};


#endif //CHAP01_CAMERA_H

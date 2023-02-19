//
// Created by 32725 on 2022/10/20.
//

#include "Camera.h"
#include <chrono>
#include <thread>
#include "glm/gtx/transform.hpp"

using namespace std::chrono;

void Camera::Turn(GLFWwindow *w, double x, double y) {
    Camera *camera = (Camera *) glfwGetWindowUserPointer(w);
    double dx = x - camera->lsX;
    double dy = camera->lsY - y;
    if(camera->MoveMode)
        camera->scrOffset=glm::vec2{dx,dy}/(glm::vec2(camera->width,camera->height)*100.f);
    else
        camera->scrOffset=glm::vec2{0,0};
    if (camera->WalkMode) {
        camera->thY -= dy * camera->lmd;
        camera->thX += dx * camera->lmd;
        if (camera->thY < 0.15f)
            camera->thY = 0.15f;
        else if (camera->thY > 3.1f)
            camera->lsY = 3.1f;
        camera->lookat = glm::normalize(
                vec3{-sin(camera->thY) * sin(camera->thX), cos(camera->thY), sin(camera->thY) * cos(camera->thX)});
        camera->scrHorizon=glm::normalize(glm::cross(camera->lookat,camera->up));
        camera->scrVertical=glm::normalize(glm::cross(camera->scrHorizon,camera->lookat));
    }
    camera->lsX = x, camera->lsY = y;
}

void Camera::Walk(GLFWwindow *w) {
    auto start = steady_clock::now(),lastShot=start;
    Camera *camera = (Camera *) glfwGetWindowUserPointer(w);
    while (!glfwWindowShouldClose(w)) {
        if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(w, 1);
            return;
        }else if(auto res=glfwGetMouseButton(w,GLFW_MOUSE_BUTTON_RIGHT);res==GLFW_PRESS){
            camera->WalkMode=true;
        }else if(res==GLFW_RELEASE){
            camera->WalkMode=false;
        }
        if(duration_cast<seconds>(steady_clock::now()-lastShot).count()>=2&& glfwGetKey(w,GLFW_KEY_P)==GLFW_PRESS){
            lastShot=steady_clock::now();
            camera->ScreenShot=true;
        }
        if (camera->WalkMode) {
            if (duration_cast<milliseconds>(steady_clock::now() - start).count() >= 5) {
                if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) {
                    camera->origin += camera->lookat * camera->lmd;
                } else if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) {
                    camera->origin -= camera->lookat * camera->lmd;
                }
                if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) {
                    camera->origin -= glm::normalize(glm::cross(camera->lookat, vec3{0, 1, 0})) * camera->lmd;
                } else if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) {
                    camera->origin -= glm::normalize(glm::cross(camera->lookat, vec3{0, -1, 0})) * camera->lmd;
                }
                if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS) {
                    camera->origin += vec3{0, 1, 0} * camera->lmd;
                } else if (glfwGetKey(w, GLFW_KEY_R) == GLFW_PRESS) {
                    camera->origin += vec3{0, -1, 0} * camera->lmd;
                }
                start=steady_clock::now();
            }

        }
    }
}

void Camera::walking(GLFWwindow *w) {
    (new std::thread{Camera::Walk,w})->detach();
}

void Camera::ResizeCall(GLFWwindow *w, int wid, int hig) {
    glViewport(0,0,wid,hig);
    auto camera=(Camera*) glfwGetWindowUserPointer(w);
    camera->width=wid/100.f;
    camera->height=hig/100.f;
    camera->Resize=true;
    camera->scrSize=glm::vec2{camera->width/4/tan(camera->fov/2),camera->height/4/tan(camera->fov/2)};
}

glm::mat4 Camera::GetTransform(const vec3& offset)const {
    return glm::perspective(fov, width / height, .05f, 100.f) *
                                glm::lookAt(origin-offset, origin + lookat-offset, up);
}


void Camera::Scroll(GLFWwindow *w, double, double y) {
    auto camera= (Camera*)glfwGetWindowUserPointer(w);
    camera->fov+=y*5E-1;
    if(camera->fov<30.f)
        camera->fov=30.f;
    else if(camera->fov>150.f)
        camera->fov=150.f;
    camera->scrSize=glm::vec2{camera->width/4/tan(camera->fov/2),camera->height/4/tan(camera->fov/2)};
}

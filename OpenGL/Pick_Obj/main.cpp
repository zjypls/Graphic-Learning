
#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "svpng/svpng.inc"
#include "glm/gtc/type_ptr.hpp"
#include "Camera.h"
#include <thread>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Model.h"


Camera camera{glm::vec3{2, 0, 0}, glm::vec3{-1, 0, 0}, glm::vec3{0, 1, 0}, 1.5f, 1.5f, 8.f, 6.f, 1E-2, 60.f, 0, 0,
              false, false, false, false, glm::vec2{2 / tan(.4f), 1.5 / tan(.4f)}, glm::vec2{0, 0},
              vec3{0, 0, -1}, vec3{0, 1, 0}};


using glm::vec3;


void Shot(float *p) {
    FILE *file = fopen("shot.png", "wb");
    assert(file != nullptr);
    auto Pixels = (unsigned char *) malloc(camera.width * 100 * camera.height * 100 * 3);
    for (int i = 0; i < camera.height * 100; ++i)
        for (int j = 0; j < camera.width * 100 * 3; ++j)
            Pixels[i * int(camera.width * 100 * 3) + j] =
                    p[int(camera.height * 100 - i - 1) * int(camera.width * 100 * 3) + j] * 255;
    svpng(file, camera.width * 100, camera.height * 100, Pixels, 0);

    fclose(file);
    free(Pixels);
    free(p);
    std::cout << "Shot Finished" << std::endl;
}

int main() {
    Model::Model model{std::string{RootPath}+"/Model/cube.obj", 1, glm::vec3{-9, 0, 0}};

    glfwInit();

    GLFWwindow *window = glfwCreateWindow(800, 600, "Pick", nullptr, nullptr);
    {
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, &camera);
        glfwGetCursorPos(window, &camera.lsX, &camera.lsY);
        glfwSetCursorPosCallback(window, Camera::Turn);
        glfwSetFramebufferSizeCallback(window, Camera::ResizeCall);
        glfwSetScrollCallback(window, Camera::Scroll);
        gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
    }


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    Shader geom{}, pick{};
    GLint indexPic;
    {
        pick.Add(std::string{RootPath}+"/Shaders/Pick/pick.vert", GL_VERTEX_SHADER);
        pick.Add(std::string{RootPath}+ "/Shaders/Pick/pick.frag", GL_FRAGMENT_SHADER);
        pick.Link();
        pick.Use();
        geom.Add(std::string{RootPath}+"/Shaders/Pick/vert.vert", GL_VERTEX_SHADER);
        geom.Add(std::string{RootPath}+"/Shaders/Pick/frag.frag", GL_FRAGMENT_SHADER);
        geom.Link();
        geom.Use();
        indexPic = geom.GetUniform("TargetIndex");
    }

    unsigned picFBO, picDep, picCol;
    {
        glGenFramebuffers(1, &picFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, picFBO);
        glGenTextures(1, &picCol);
        glBindTexture(GL_TEXTURE_2D, picCol);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 800, 600, 0, GL_RGBA, GL_FLOAT, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picCol, 0);

        glGenTextures(1, &picDep);
        glBindTexture(GL_TEXTURE_2D, picDep);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, picDep, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return -14154;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    model.GenerateVAO();

    glEnable(GL_DEPTH_TEST);
    (new std::thread{Camera::Walk, window})->detach();
    glm::vec3 index = glm::vec3{-1, -1, -1};
    float *pixel = new float[4];

    while (!glfwWindowShouldClose(window)) {
        if (camera.Resize) {
            camera.Resize = false;
            {
                glDeleteTextures(1, &picCol);
                glDeleteTextures(1, &picDep);
                glDeleteFramebuffers(1, &picFBO);
            }
            {
                glGenFramebuffers(1, &picFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, picFBO);
                glGenTextures(1, &picCol);
                glBindTexture(GL_TEXTURE_2D, picCol);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, camera.width * 100, camera.height * 100, 0, GL_RGBA,
                             GL_FLOAT, nullptr);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picCol, 0);

                glGenTextures(1, &picDep);
                glBindTexture(GL_TEXTURE_2D, picDep);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, camera.width * 100, camera.height * 100,
                             0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, picDep, 0);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }
        if (camera.WalkMode) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            index.z = -1;
            goto paint;
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            camera.MoveMode = true;
            index.z = 1;
            goto paint;
        } else if (camera.MoveMode = false, index = glm::vec3{-1, -1, -1};false) {
            paint:
            if (!camera.MoveMode || index.y < 0.5) {
                glBindFramebuffer(GL_FRAMEBUFFER, picFBO);
                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            glBindVertexArray(VAO);
                pick.Use();
                model.Draw(pick, camera);

                glReadPixels(camera.lsX, camera.height * 100 - camera.lsY, 1, 1, GL_RGBA, GL_FLOAT, pixel);

                index = glm::vec3{pixel[0], pixel[1], index.z};
                //std::cout<<index.x<<":"<<index.y<<":"<<index.z<<std::endl;
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            if (index.y > 0.5) {
                model.Move(camera);
            }
        }
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("set");
            ImGui::DragFloat3("Model Offset", glm::value_ptr(model.origin), .08);
            ImGui::DragFloat3("Camera Position:",glm::value_ptr(camera.origin),.05);
            ImGui::Text("Tip:\nRight Mouse Button:Move Mode(W,S,A,D,SPACE,R)\n");
            ImGui::Text("Mid Button:choose and move Mode");
            ImGui::Text("Left Button:Fragment choose Mode");
            ImGui::Text("Key P:Screen Shot");
            ImGui::End();
            ImGui::Render();
        }

        geom.Use();
        //glUniform1f(indexPic,pixel[0]);
        glUniform3fv(indexPic, 1, glm::value_ptr(index));
        model.Draw(geom, camera);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (camera.ScreenShot) {
            camera.ScreenShot = false;
            float *ShotPixels = (float *) malloc(camera.width * 100 * camera.height * 100 * sizeof(float) * 3);
            glReadPixels(0, 0, camera.width * 100, camera.height * 100, GL_RGB, GL_FLOAT, ShotPixels);
            (new std::thread{Shot, ShotPixels})->detach();
        }
    }

    delete[] pixel;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

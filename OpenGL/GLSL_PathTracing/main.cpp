#include <iostream>
#include "RayShader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include <chrono>
#include <random>

std::uniform_int_distribution<int> uid(1,1<<20);
std::default_random_engine dre;
Camera camera{vec3(0,0,1),vec3(4,0,0),vec3(0,3,0),vec3(-2,-1.5,-1),vec3{1,1,1},vec3{0,0,-1},1.57,0,2,0.001};
int depth=8;
float msaa=4.0f;
double lstX=400,lstY=300;
unsigned FBO[2],texture[2],rbo[2];
bool viewportChange=false,rightClick= false;
int Index=1,ObjIndex=0,MatIndex=2;
const float vert[]={
        1,1,0,
        -1,-1,0,
        1,-1,0,

        -1,1,0,
        1,1,0,
        -1,-1,0
};
float horizon=4,vertical=3;
using namespace std::chrono;
decltype(steady_clock::now()) preTime{};
void CursorCall(GLFWwindow*,double x,double y){
    if(rightClick){
        camera.Update(x-lstX,y-lstY);
        Index=1;
    }
    lstX=x,lstY=y;
}
vec3 colors[]{vec3{1},vec3{1},vec3{1},vec3{1},vec3{1},vec3{1},vec3{1},vec3{1},vec3{1},vec3{1}};
int Mats[]{LIGHT,LAMBERT,LAMBERT,LAMBERT,LAMBERT,LAMBERT,LAMBERT,MIRR,LAMBERT,GLASS};
bool Repaint=false,lighting=true;
void viewport(GLFWwindow* w,int wid,int hig){
    while(wid==0||hig==0){
        glfwGetFramebufferSize(w,&wid,&hig);
        glfwWaitEvents();
    }
    glViewport(0,0,wid,hig);
    horizon=float(wid)/200;
    vertical=float(hig)/200;
    camera.horizon=camera.horizon*(horizon/camera.horizon.length());
    camera.vertical=camera.vertical*(vertical/camera.vertical.length());
    camera.leftcorner=camera.ForWord*camera.focus-camera.horizon*0.5f-camera.vertical*0.5f;
    Index=1;
    for(int i=0;i<2;++i){
        glBindFramebuffer(GL_FRAMEBUFFER,FBO[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hig, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture[i],0);

        glBindRenderbuffer(GL_RENDERBUFFER,rbo[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, wid, hig);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo[i]);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
            throw runtime_error{"FrameBuffer not complete"};
        }
    }
    //(GL_FRAMEBUFFER,0);
    viewportChange=true;
}
void KeyCall(GLFWwindow *,int,int,int,int);
const char* objects[]{"Top Light","Top Wall","Left Wall","Right Wall","Back Wall","BackGround","Ball 0","Ball 1","Ball 2","Ball 3"};
const char* SurfaceType[]{"LAMBERT","MIRR","LIGHT","GLASS"};
int main(){
    glfwInit();
    //glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
    GLFWwindow* window= glfwCreateWindow(800,600,"Tracing", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetCursorPos(window,lstX,lstY);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window,viewport);
    glfwSetCursorPosCallback(window,CursorCall);
    glfwSetKeyCallback(window,KeyCall);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;
    unsigned VAO,VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER,sizeof(vert),vert,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,0);
    glEnableVertexAttribArray(0);

    glGenFramebuffers(2,FBO);
    glGenTextures(2,texture);
    glGenRenderbuffers(2,rbo);
    for(int i=0;i<2;++i){
        glBindFramebuffer(GL_FRAMEBUFFER,FBO[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture[i],0);

        glBindRenderbuffer(GL_RENDERBUFFER,rbo[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo[i]);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
            throw runtime_error{"Frame Buffer not complete"};
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    RayShader shader{};
    shader.Add(std::string{RootPath}+"/Shaders/Ray/vert.glsl",GL_VERTEX_SHADER);
    shader.Add(std::string{RootPath}+ "/Shaders/Ray/nfrag.glsl",GL_FRAGMENT_SHADER);
    shader.Link();
    RayShader text{};
    text.Add(std::string{RootPath}+"/Shaders/Ray/vert.glsl",GL_VERTEX_SHADER);
    text.Add(std::string{RootPath}+"/Shaders/Ray/TextShader.glsl",GL_FRAGMENT_SHADER);
    text.Link();
    shader.Use();

    shader.Set("camera.horizon",camera.horizon);
    shader.Set("camera.vertical",camera.vertical);
    shader.Set("camera.leftcorner",camera.leftcorner);

    shader.Set("depth",depth);
    shader.Set("msaa",msaa);
    shader.Set("world.count1",5);
    shader.Set("world.count2",5);
    shader.AddObject(Sphere{vec3(0,0,-1.0),0.5f,LAMBERT,colors[1+5]},1);
    shader.AddObject(Sphere{vec3(0,-100.5,-0.5),100.0f,LAMBERT,colors[5]},0);
    shader.AddObject(Sphere{vec3(-0.6,-0.1,-0.1),0.4f,MIRR,colors[2+5]},2);
    shader.AddObject(Sphere{vec3(0.55,-0.2,-0.2),0.3f,LAMBERT,colors[3+5]},3);
    shader.AddObject(Sphere{vec3(0.1,0.4,-0.1),0.3f,GLASS,colors[4+5]},4);


    shader.AddObject(Face{0.8,1.6,-0.6,2,-3,0.5,LAMBERT,vec3(-1,0,0),vec3(1.4,0.5,-0.5),colors[3]},3);
    shader.AddObject(Face{-1.6,-0.8,-0.6,2,-3,0.5,LAMBERT,vec3(1,0,0),vec3(-1.55,0.5,-0.5),colors[2]},2);
    shader.AddObject(Face{-1.6,1.8,1.4,2.5,-3,0.5,LAMBERT,vec3(0,-1,0),vec3(1.1,2,-0.5),colors[1]},1);
    shader.AddObject(Face{-0.4,0.4,1.4,2.5,-1.4,-0.6,LIGHT,vec3(0,-1,0),vec3(1.1,1.99,-0.5),colors[0]},0);
    shader.AddObject(Face{-1.6,1.8,-0.6,2,-3,-1.5,LAMBERT,vec3(0,0,1),vec3(1.4,0.5,-2),colors[4]},4);

    shader.Set("camera.origin",camera.origin);
    auto start=steady_clock::now(),end=start;

    while(!glfwWindowShouldClose(window)){
        start=steady_clock::now();
        if(duration_cast<milliseconds>(start-end).count()<10){
            continue;
        }
        if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS){
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
            rightClick=true;
        }else{
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
            rightClick=false;
        }
        glfwPollEvents();
        glBindFramebuffer(GL_FRAMEBUFFER,FBO[Index%2]);
        shader.Use();
        if(viewportChange||rightClick) {
            shader.Set("camera.origin",camera.origin);
            shader.Set("camera.horizon", camera.horizon);
            shader.Set("camera.vertical", camera.vertical);
            shader.Set("camera.leftcorner", camera.leftcorner);
            viewportChange=false;
        }
        shader.Set("Rdu",Index);
        glBindTexture(GL_TEXTURE0,texture[(Index+1)%2]);
        glDrawArrays(GL_TRIANGLES,0,6);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        text.Use();
        glBindTexture(GL_TEXTURE0,texture[Index%2]);
        glDrawArrays(GL_TRIANGLES,0,6);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Settings");
//        ImGui::Text("Time/Frame:%ld",(duration_cast<milliseconds>(end-start).count()));
        ImGui::Text("Samplers:%d",Index);
        if(ImGui::InputInt("Ray Depth",&depth)){
            if(depth<0)
                depth=1;
            else if(depth>256)
                depth=256;
            shader.Use();
            shader.Set("depth",depth);
            goto repaint;
        }
        if(ImGui::InputFloat("MSAA",&msaa)){
            if(msaa>128)
                msaa=128;
            else if(msaa<0)
                msaa=1;
            shader.Use();
            shader.Set("msaa",msaa);
            goto repaint;
        }
        ImGui::Combo("Objects",&ObjIndex,objects,10);
        if(ImGui::ColorEdit3("Color",const_cast<float*>(colors[ObjIndex].ptr()))){
            shader.Use();
            if(ObjIndex<5) {
                shader.Set((string("world.faces[") + to_string(ObjIndex) + "].color").c_str(),colors[ObjIndex]);
            }else{
                shader.Set((string("world.spheres[") + to_string(ObjIndex-5) + "].color").c_str(),colors[ObjIndex]);
            }
            goto repaint;
        }
        if(ImGui::Checkbox("Lighting",&lighting)){
            shader.Use();
            if(lighting){
                if(ObjIndex<4) {
                    shader.Set((string("world.faces[") + to_string(ObjIndex) + "].type").c_str(),LIGHT);
                }else{
                    shader.Set((string("world.spheres[") + to_string(ObjIndex-5) + "].type").c_str(),LIGHT);
                }
            }else{
                if(ObjIndex<4) {
                    shader.Set((string("world.faces[") + to_string(ObjIndex) + "].type").c_str(),LAMBERT);
                }else{
                    shader.Set((string("world.spheres[") + to_string(ObjIndex-5) + "].type").c_str(),LAMBERT);
                }
            }
            goto repaint;
        }
        if(ImGui::Combo("Material",&MatIndex,SurfaceType,4)){
            shader.Use();
            if(ObjIndex<5) {
                shader.Set((string("world.faces[") + to_string(ObjIndex) + "].type").c_str(),MatIndex);
            }else{
                shader.Set((string("world.spheres[") + to_string(ObjIndex-5) + "].type").c_str(),MatIndex);
            }
            goto repaint;
        }

        if(Repaint){
            Repaint=false;
            repaint:

            for(auto i:FBO){
                glBindFramebuffer(GL_FRAMEBUFFER,i);
                glClearColor(0,0,0,0);
                glClear(GL_COLOR_BUFFER_BIT);
            }
            glBindFramebuffer(GL_FRAMEBUFFER,0);
            Index=1;
        }

        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ++Index;

        end=steady_clock::now();
        glfwSwapBuffers(window);
    }
    glDeleteFramebuffers(2,FBO);
    glDeleteRenderbuffers(2,rbo);
    glDeleteTextures(2,texture);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void KeyCall(GLFWwindow *w,int key,int scanmode,int action,int mode){
    if(key==GLFW_KEY_ESCAPE){
        glfwSetWindowShouldClose(w,1);
        return;
    }
    vec3 walkVec{0,0,0};
    if(rightClick&&duration_cast<milliseconds>(steady_clock::now()-preTime).count()>20) {
        Index=1;
        switch (key) {
            case GLFW_KEY_W:
                walkVec = camera.ForWord;
                break;
            case GLFW_KEY_A:
                walkVec = walkVec - (camera.horizon).normalize();
                break;
            case GLFW_KEY_D:
                walkVec = (camera.horizon).normalize();
                break;
            case GLFW_KEY_S:
                walkVec =walkVec-(camera.ForWord).normalize();
                break;
            case GLFW_KEY_SPACE:
                walkVec = vec3(0, 1, 0);
                break;
        }
    }
    camera.origin=camera.origin+walkVec*0.01;
}
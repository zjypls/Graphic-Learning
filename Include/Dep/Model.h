//
// Created by 32725 on 2022/11/19.
//

#ifndef TELLI_GL_MODEL_H
#define TELLI_GL_MODEL_H

#include "glm/glm.hpp"
#include <vector>
#include <string>
#include "glad/glad.h"
#include "Camera.h"
#include "Shader.h"

namespace Model {
    using namespace glm;
    using namespace std;
    struct Vertex {
        vec3 vert;
        vec3 normal;
        vec2 tex;
        float index;

        bool operator==(const Vertex&v){
            return vert==v.vert&&normal==v.normal&&tex==v.tex;
        }
    };

    class Model {
    public:
        uint32_t index;
        vec3 origin;
        vector<Vertex> vertexes;
        vector<uint32_t> indices;
        GLuint VAO,VBO,EBO;

        Model(const string&,uint32_t,const vec3&offset=glm::vec3{0,0,0});
        void GenerateVAO();
        void Draw(const Shader&,const Camera&)const;
        inline void Move(Camera&camera) {
            float Dx= length(origin-camera.origin);
            origin+=Dx*(camera.scrHorizon*camera.scrOffset.x+camera.scrVertical*camera.scrOffset.y);
            camera.scrOffset=glm::vec2{0,0};
        }
    };

}


#endif //TELLI_GL_MODEL_H

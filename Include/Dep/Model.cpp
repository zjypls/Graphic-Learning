//
// Created by 32725 on 2022/11/19.
//
#define TINYOBJLOADER_IMPLEMENTATION
#include "Model.h"
#include "tinyobjloader/tiny_obj_loader.h"
//#include "glm/gtx/hash.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <unordered_map>


namespace Model {
    /*namespace std{
        template<>
        struct hash<Vertex>{
            uint64_t operator()(const Vertex&v)const{
                return (hash<vec3>()(v.vert)<<2)|(hash<vec3>()(v.normal)>>2)
                        |(hash<vec2>()(v.tex)<<1)|hash<uint32_t>()(v.index);
            }
        };
    }*/
    Model::Model(const string& path,uint32_t I,const vec3&offset):index{I},origin{offset} {
        tinyobj::attrib_t attrib;
        vector<tinyobj::shape_t> shapes;
        string warn,error;
        if(!tinyobj::LoadObj(&attrib,&shapes, nullptr,&warn,&error,path.c_str())){
            throw runtime_error{warn+error};
        }
        for(const auto&shape:shapes) {
            for (int i = 0; i < shape.mesh.indices.size(); ++i) {
                auto index = shape.mesh.indices[i];
                Vertex vert;
                vert.vert = vec3{
                        attrib.vertices[index.vertex_index * 3 + 0],
                        attrib.vertices[index.vertex_index * 3 + 1],
                        attrib.vertices[index.vertex_index * 3 + 2]};
                vert.normal = vec3{
                        attrib.normals[index.normal_index * 3 + 0],
                        attrib.normals[index.normal_index * 3 + 1],
                        attrib.normals[index.normal_index * 3 + 2]};
                vert.tex = glm::vec2{
                        attrib.texcoords[index.texcoord_index * 2 + 0],
                        attrib.texcoords[index.texcoord_index * 2 + 1]};
                vert.index=i/3+1;
                vertexes.push_back(vert);
            }
        }
    }

    void Model::GenerateVAO() {
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,vertexes.size()*sizeof(Vertex),vertexes.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(sizeof(vec3)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<const void *>(sizeof(vec3)*2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<const void *>(sizeof(vec3)*2+sizeof(vec2)));
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }

    void Model::Draw(const Shader&shader,const Camera& camera)const {
        auto mvp=camera.GetTransform(origin);
        glBindVertexArray(VAO);
        glUniformMatrix4fv(shader.GetUniform("MV"),1,GL_FALSE, value_ptr(mvp));
        glUniform1i(shader.GetUniform("modelIndex"),int(index));
        glDrawArrays(GL_TRIANGLES,0,vertexes.size());
    }



}
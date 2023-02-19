//
// Created by 32725 on 2022/11/25.
//

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <cassert>

Shader::Shader() {
    Program=glCreateProgram();
}

void Shader::Add(const std::string & _shaderPath,GLenum _shaderType) {
    std::fstream file(_shaderPath.c_str());
    if(!file.is_open())
        throw std::runtime_error{"illegal path:"+_shaderPath};
    std::stringstream stream;
    stream<<file.rdbuf();
    std::string str=stream.str();
    const char*code=str.c_str();

    GLuint shader=glCreateShader(_shaderType);
    glShaderSource(shader,1,&code, nullptr);
    glCompileShader(shader);
    int suc=0;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&suc);
    assert(suc);
    glAttachShader(Program,shader);
    glDeleteShader(shader);
}

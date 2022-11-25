//
// Created by 32725 on 2022/11/25.
//

#ifndef GRAPHICS_SHADER_H
#define GRAPHICS_SHADER_H

#include <string>
#include "../glad/glad.h"
#include "../GLFW/glfw3.h"

class Shader {
protected:
    unsigned Program;
public:
    Shader();
    void Add(const std::string&,GLenum);
    inline void Link(){glLinkProgram(Program);};
    inline void Use(){glUseProgram(Program);}
    inline GLuint GetUniform(const char* name){return glGetUniformLocation(Program, name);};

};


#endif //GRAPHICS_SHADER_H

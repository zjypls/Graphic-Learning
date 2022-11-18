#include "Shader.h"

Shader::Shader(const string &vert, const string &frag, const string geom) {
    ifstream file;
    stringstream midfile;
    string data;
    const char *source;
    unsigned shader;
    int success;

    file.open(vert, ios::binary);
    if (!file.is_open()) {
        throw runtime_error{"vert path error"};
    }
    midfile << file.rdbuf();
    data = midfile.str();
    source = data.c_str();

    shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        ofstream log{"vert.log", ios::trunc};
        char message[512];
        glGetShaderInfoLog(shader, 512, NULL, message);
        log << message;
        throw runtime_error{"vert error"};
    }
    Program = glCreateProgram();
    glAttachShader(Program, shader);
    glDeleteShader(shader);
    data.clear();
    midfile.str("");
    file.close();


    file.open(frag, ios::binary);
    if (!file.is_open()) {
        throw runtime_error{"frag path error"};
    }
    midfile << file.rdbuf();
    data = midfile.str();
    source = data.c_str();

    shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        ofstream log{"frag.log", ios::trunc};
        char message[512];
        glGetShaderInfoLog(shader, 512, NULL, message);
        log << message;
        log.close();
        throw runtime_error{"frag error"};
    }
    glAttachShader(Program, shader);
    glDeleteShader(shader);
    data.clear();
    midfile.str("");
    file.close();
    if (geom != "") {
        file.open(geom, ios::binary);
        if (!file.is_open()) {
            throw runtime_error{"geom path error"};
        }
        midfile << file.rdbuf();
        data = midfile.str();
        source = data.c_str();

        shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            ofstream log{"geom.log", ios::trunc};
            char message[512];
            glGetShaderInfoLog(shader, 512, NULL, message);
            log << message;
            log.close();
            throw runtime_error{"geom error"};
        }
        glAttachShader(Program, shader);
        glDeleteShader(shader);
        data.clear();
        midfile.str("");
        file.close();
    }
    glLinkProgram(Program);
}

void Shader::Set(const char *name, float value) {
    glUniform1f(glGetUniformLocation(Program, name), value);
}

void Shader::Set(const char *name, int value) {
    glUniform1i(glGetUniformLocation(Program, name), value);
}

void Shader::Set(const char *name, const vec3 &value) {
    glUniform3fv(glGetUniformLocation(Program, name), 1, value.ptr());
}

void Shader::AddObject(const Sphere &sphere, const int index) {
//    char name[18]="world.spheres[0].";
//    sprintf(name,"world.spheres[%d].",index);
    string name = string("world.spheres[") + to_string(index) + "].";
    Set((name + "center").c_str(), sphere.center);
    Set((name + "radius").c_str(), sphere.radius);
    Set((name + "type").c_str(), sphere.type);
    Set((name + "color").c_str(), sphere.color);
}

void Shader::AddObject(const Face &face, const int index) {
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

#version 460
layout(location=0)out vec4 FragC;

layout(location=0)in vec2 idx;

void main() {
    FragC=vec4(idx,-1,-1);
}
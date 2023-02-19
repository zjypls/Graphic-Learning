#version 460

layout(location=0)out vec4 FragC;
layout(location=0)in vec3 N;

void main(){
    FragC=vec4(N,1);
}
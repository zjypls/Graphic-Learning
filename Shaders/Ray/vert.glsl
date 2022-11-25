#version 450
layout(location=0)in vec3 pos;
out vec2 fpos;

void main() {
    fpos=((pos+1)/2).xy;
    gl_Position=vec4(pos,1);
}

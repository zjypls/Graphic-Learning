#version 460
layout(location=0)in vec3 pos;
layout(location=1)in vec3 N;
layout(location=2)in vec2 Tex;
layout(location=3)in float index;

uniform mat4 MV;
uniform int modelIndex;
layout(location=0)out vec2 idx;

void main() {
    gl_Position=MV*vec4(pos,1);
    idx=vec2(index,modelIndex);
}
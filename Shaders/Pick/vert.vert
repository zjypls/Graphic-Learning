#version 460

layout(location=0)in vec3 pos;
layout(location=1)in vec3 Normal;
layout(location=2)in vec2 tex;
layout(location=3)in float index;

uniform mat4 MV;
uniform int modelIndex;
uniform vec3 TargetIndex;
layout(location=0)out vec3 N;


void main(){
    gl_Position=MV*vec4(pos,1);
    N=Normal;
    if(TargetIndex.z>0){
        if(abs(TargetIndex.y-modelIndex)<1E-1){
            N=1-N;
        }
        return;
    } else if(abs(TargetIndex.x-index)<1E-2)
        N=1-N;
}
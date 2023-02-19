#version 450

out vec4 FragColor;
in vec2 fpos;
uniform sampler2D now;
void main() {
    FragColor=texture(now,fpos);
}

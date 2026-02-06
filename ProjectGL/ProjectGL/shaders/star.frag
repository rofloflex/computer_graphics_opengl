#version 460 core
out vec4 FragColor;

uniform float uTime;

void main()
{
    float pulse = sin(uTime) * 0.5 + 0.5;
    FragColor = vec4(0.7, pulse, 0.3, 1.0);
}
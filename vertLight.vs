#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;


void main()
{
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    //Normal = aNormal;
    Normal = mat3(transpose(inverse(view * model))) * aNormal;//  - multiply the normal vector by a normal matrix - important if we work in non uniform but not efficient at all.

    LightPos = vec3(view * vec4(lightPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
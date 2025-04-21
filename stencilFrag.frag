#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth){ // we transform the non linear depth buffer to a linear one
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / ( far + near - z*(far-near)); //derived from the projection matrix for non linearizing the depths values
}


void main()
{    
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    //FragColor = vec4(vec3(depth), 1.0);
    FragColor = mix(vec4(0.2,0.2,0.2, 1.0),  vec4(vec3(depth), 0.0), 0.8);


}


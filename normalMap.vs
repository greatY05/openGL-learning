

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;

	//create the tbn matrix
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	//gram schmidting to re orthogonalize the tbn vectors such taht each vector is perpendicular to the other verctors again.
	T = normalize(T- dot(T,N) * N);
	vec3 B = cross (N,T);

	//because normal maps operate on xyz coords, they are not translatable to other rotations, so we use TBN matrix to transform the map coords into a tangent space coords
	//(tbn stands for tangent bitangent normal)
	//we now need to choose etiher to gtet tangent space to the light variables or the light variables to tangent space(normal map)
	//although it might seem like less work, option one is inferior, due to it requiring to run more on the fragment shader (having to do matrix multiplication there)
	//so we choose the second option to transfer the light variables to tangent space and send them to the fragment 
	mat3 TBN = transpose(mat3(T,B,N));
	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
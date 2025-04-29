

#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 

    //number o depth layers
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

    //calc the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    //the amount to shift the texture coords per layer (from vector P)
    vec2 P = viewDir.xy * heightScale;
    vec2 deltaTexCoords = P/numLayers;
    
    //steep parallax mapping


    //get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        //shift the texture coords along the direction of P vector
        currentTexCoords -= deltaTexCoords;
        //get depthmap value at current texture coords
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
        //get depth of next layer
        currentLayerDepth += layerDepth;
    }
     

     //parallax occlusion mapping

     //get the texture coords before collision (reverse operation)
     vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
     
     //get depth after and bfore collision for linear interpolation
     float afterDepth = currentDepthMapValue - currentLayerDepth;
     float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;

     //interpolation of texture coordinates
     float weight = afterDepth / (afterDepth - beforeDepth);
     vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

     return finalTexCoords;
      
}

void main()
{           
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    
    texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir); 
    //discard if outside of og texture coords to avoid weird wrapping looking thing
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // obtain normal from normal map
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    // get diffuse color
    vec3 color = texture(diffuseMap, texCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}


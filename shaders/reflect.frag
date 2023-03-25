#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;
in vec4 fragmentPosEyeSpace;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightDir2;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform int fogen;
uniform int draw_reflect;
uniform samplerCube skybox;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float computeFog()
{
 float fogDensity = 0.02f;
 float fragmentDistance = length(fragmentPosEyeSpace);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}
float computeShadow() {
	vec3 normalizedCoords = fragmentPosEyeSpace.xyz / fragmentPosEyeSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = max(0.05f*(1.0f - dot(fNormal, lightDir)), 0.005f);
	float shadow = currentDepth - bias> closestDepth ? 1.0 : 0.0;
	
	return shadow;
}
void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDirN = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;


    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

void main() 
{
    computeDirLight();
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

    //compute final vertex color
  	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	vec4 coloraux = vec4(color.x,color.y,color.z,1.0f);
    
    float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	vec3 viewDirectionN = normalize(fragmentPosEyeSpace.xyz); 
	vec3 normalN = normalize(fNormal); 
	vec3 reflection = reflect(viewDirectionN, normalN); 
	vec3 colorFromSkybox = vec3(texture(skybox, reflection));
    fColor = vec4(colorFromSkybox,1.0f);
		
		
}

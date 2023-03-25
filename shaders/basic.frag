#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec3 vPos;

out vec4 fColor;
in vec4 fragmentPosEyeSpace;
in vec4 fPosEye;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lanterna;
uniform vec3 lanternaCol;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform int fogen;

//constants
float constant = 0.5f;
float linear = 0.3f;
float quadratic = 0.2f;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
//componente lumina flashlight
uniform vec3 position;
uniform vec3 direction;
uniform vec3 cutOff;

float computeFog()
{
 float fogDensity = 0.02f;
 float fragmentDistance = length(fPosEye);
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
    vec3 normalEye = normalize(fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDirN = normalize(lightDir);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;


    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;

	//reflector
	lightDirN = normalize(lanterna-vPos.xyz);
	float dist = length(lanterna - vPos.xyz);
	float att = (1.0f / (constant + linear * dist + quadratic * (dist * dist)))*100;


	ambient += att * ambientStrength * lanternaCol;
	diffuse += att * max(dot(normalEye, lightDirN), 0.0f) * lanternaCol;
	}
	

	//lanterna spotlight
	

void main() 
{
    computeDirLight();
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

    //compute final vertex color
  	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	vec4 coloraux = vec4(color.x,color.y,color.z,0.2f);
    
    float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	if(fogen == 1)
	fColor = fogColor * (1 - fogFactor) + coloraux * fogFactor;
	else
	fColor = vec4(color,0.6f);
		
		
}

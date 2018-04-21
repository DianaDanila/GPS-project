#version 410 core

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

out vec4 fColor;
in vec2 fragTexCoords;
in vec3 fragNormal;
in vec3 fragPosition;
in vec4 fragPosEye;
in vec3 fragFog;

in vec4 fragPosLightSpace;

uniform mat3 normalMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat3 lightDirMatrix;
uniform vec3 cameraPosition;


struct directionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 color;
};

struct pointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

struct fog{
	vec3 position;
	vec4 color;
	float density;
	//float factor;
};

uniform directionalLight myLight;
uniform pointLight myPointLight;
uniform fog myFog;
uniform fog myFogDark;

vec3 ambient = vec3(0.0f);
vec3 diffuse = vec3(0.0f);
vec3 specular = vec3(0.0f);


float shininess = 100.0f;

float computeFog(){
	float fogFactor = 1.0f;

	vec3 fogPosEye = vec3(view * vec4(myFog.position, 1.0f));
	vec3 fragPosWorld = vec3(inverse(view) * fragPosEye);
	float distance = 2.0f;
	if(fragPosWorld.x < myFog.position.x + 5.5f && fragPosWorld.x > myFog.position.x - 5.5f && fragPosWorld.z < myFog.position.z + 5.5f && fragPosWorld.z > myFog.position.z - 5.5f)
		fogFactor = exp(-pow(distance * myFog.density, 2));

	return fogFactor;
}

float computeFogDark(){
	float fogFactor = 1.0f;

	vec3 fogPosEye = vec3(view * vec4(myFogDark.position, 1.0f));
	vec3 fragPosWorld = vec3(inverse(view) * fragPosEye);
	float distance = 2.0f;
	if(fragPosWorld.x < myFogDark.position.x + 5.5f && fragPosWorld.x > myFogDark.position.x - 5.5f && fragPosWorld.z < myFogDark.position.z + 5.5f && fragPosWorld.z > myFogDark.position.z - 5.5f)
		fogFactor = exp(-pow(distance * myFogDark.density, 2));

	return fogFactor;
}


float ShadowCalculation()
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
		float bias = 0.005f;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

		float shadow = 0.0f;

    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; x++){
        for (int y = -1; y <= 1; y++){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }

    return shadow / 9.0f;
}

void CalcPointLight()
{
	vec3 normalEye = normalize(fragNormal);

	//compute light pos in eye corods
	vec3 lightPosEye = vec3(view * vec4(myPointLight.position, 1.0f));
	//normalize light direction
	vec3 lightDirN = normalize(lightPosEye - fragPosEye.xyz);

	float distance = length(lightPosEye - fragPosEye.xyz);
	float att = 1.0f / (myPointLight.constant + myPointLight.linear * distance + myPointLight.quadratic * distance * distance);
	//compute view direction (in eye coordinates, the viewer is situated at the origin
	vec3 viewDirN = normalize(- fragPosEye.xyz);

	//compute ambient light
	ambient += att * myPointLight.ambient * myPointLight.color;

	//compute diffuse light
  diffuse += att * max(dot(normalEye, lightDirN), 0.0f) * myPointLight.diffuse * myPointLight.color;

	//compute specular light
	vec3 halfVector = normalize(lightDirN + viewDirN);

	float specCoeff = pow(max(dot(viewDirN, halfVector), 0.0f), shininess);
  specular += att * specCoeff * myPointLight.specular * myPointLight.color;
}



void computeLight(){
	float myShadow = ShadowCalculation();
	ambient += myLight.ambient * myLight.color;

	vec3 lightDirN = normalize(lightDirMatrix * myLight.direction);
	vec3 normalEye = normalize(normalMatrix * fragNormal);
	diffuse += (1.0f - myShadow) * max(dot(normalEye, lightDirN), 0.0f) * myLight.diffuse * myLight.color;

	vec4 vertPosEye = view * model * vec4(fragPosition, 1.0f);
	vec3 viewDir = normalize(- vertPosEye.xyz);
	//vec3 reflectDir = normalize(reflect(-lightDir, normalEye));
	vec3 halfDir = normalize(myLight.direction + viewDir);
	float specCoeff = pow(max(dot(viewDir, halfDir), 0.0f), shininess);
	specular += (1.0f - myShadow) * specCoeff * myLight.specular * myLight.color;

}



void main() {

	computeLight();
	CalcPointLight();
	float fogFactor = 1.0f;
	vec4 fogColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	if(cameraPosition.x <= 0.0f && cameraPosition.z <= 0.0f){
			fogFactor = computeFog();
			fogColor = myFog.color;
		}
	if(cameraPosition.x >= 0.0f && cameraPosition.z >= 0.0f){
			fogFactor = computeFogDark();
			fogColor = myFogDark.color;
		}

	//fColor = mix(myFog.color, vec4(min((ambient + diffuse) * texture(diffuseTexture, fragTexCoords).rgb + specular * texture(specularTexture, fragTexCoords).rgb, 1.0f), 1.0f), fogFactor);

  fColor = fogColor * (1 - fogFactor) + vec4(min((ambient + diffuse) * texture(diffuseTexture, fragTexCoords).rgb + specular * texture(specularTexture, fragTexCoords).rgb, 1.0f), 1.0f) * fogFactor;
}

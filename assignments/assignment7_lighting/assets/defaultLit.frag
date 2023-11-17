#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

struct Light
{
	vec3 position;
	vec3 color;
};

#define MAX_NUM_OF_LIGHTS 4
uniform Light _LightsArray[MAX_NUM_OF_LIGHTS];

uniform float _ambient;
uniform float _diffuse;
uniform float _specular;
uniform float _shine;

vec3 reflectionVec;
uniform vec3 _cameraPos;
vec3 viewingAngle;
uniform bool _blinnPhong = false;

uniform sampler2D _Texture;

float diffuseFactor;
float specularFactor;

void main(){
	FragColor = texture(_Texture,fs_in.UV);

	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 baseFragRGB = FragColor.rgb;
	FragColor = vec4(0, 0, 0, 0);

	for (int i = 0; i < MAX_NUM_OF_LIGHTS; i++)
	{
		vec3 lightDir = normalize(_LightsArray[i].position - fs_in.WorldPosition);

		diffuseFactor = max(dot(normal, lightDir), 0) * _diffuse;

		viewingAngle = normalize(_cameraPos - fs_in.WorldPosition);

		if (_blinnPhong)
		{
			vec3 h = normalize(lightDir + viewingAngle);
			specularFactor = pow(max(dot(h, normal), 0), _shine);
		}
		else
		{
			reflectionVec = 2 * dot(lightDir, normal) * normal - lightDir;
			specularFactor = pow(max(dot(reflectionVec, viewingAngle), 0), _shine);
		}

		specularFactor *= _specular;

		vec3 ambientColor = _LightsArray[i].color * _ambient;
		vec3 diffuseColor = _LightsArray[i].color * diffuseFactor;
		vec3 specularColor = _LightsArray[i].color * specularFactor;

		FragColor += vec4(baseFragRGB * (ambientColor + diffuseColor + specularColor), 1);
	}

	normalize(FragColor);
}
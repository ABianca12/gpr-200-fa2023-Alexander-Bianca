#version 450
out vec4 FragColor;
in vec2 UV;

uniform float _Time;
uniform float _TimeSpeed;

uniform float _Distortion;

uniform sampler2D _Texture;
uniform sampler2D _NoiseTexture;

void main()
{
	vec2 noiseVec = vec2(UV.x + (_Time * _TimeSpeed), UV.y);

	float noise = texture(_NoiseTexture, noiseVec).r;

	vec2 uv = UV + (noise * _Distortion) - (_Distortion / 2);
	
	FragColor = texture(_Texture, uv);
}
#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

out vec2 UV;
out vec2 characterVec;

//Time
uniform float _Time;
uniform float _TimeSpeed;

void main(){
	UV = vUV;

	UV = UV * 2.0 - 0.5;

	float tx = sin(_Time * _TimeSpeed) * 0.5;
	float ty = cos(_Time * _TimeSpeed) * 0.5;

	characterVec = vec2(UV.x + tx, UV.y + ty);

	gl_Position = vec4(vPos,1.0);
}
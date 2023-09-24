#version 450
out vec4 FragColor;
in vec2 UV;
uniform float iTime;
uniform vec3 BGColor;
uniform float sunSpeed;
uniform vec3 sunColor;
uniform float hillSpeed;
uniform vec3 hillGrassColor;
uniform float stripesSpeed;
uniform vec3 stripesColor;
uniform vec2 iResolution;

float circleSDF(vec2 p, float r)
{
	return length(p) - r;
}

void mainImage (out vec4 fragColor, in vec2 fragCoord)
{
    float radius = 0.1;

    vec2 sunPos = vec2(0.5,0.5);
    sunPos.y *= (sin(iTime * sunSpeed));
    vec3 sunColor = vec3(1.0 + 0.5*sin(iTime * sunSpeed),1.0+0.5*cos(iTime * sunSpeed),0);

    vec3 hillGrassColor = vec3(0.0,1.0,0.0);
    vec3 hill2Color = vec3(0.9,0.5,0.0);

    vec2 uv = fragCoord/iResolution.xy;uv.y *= iResolution.y/iResolution.x;

    vec2 sunBlurPos = vec2(0.5,0.5);
    sunBlurPos.y *= (sin(iTime * sunSpeed));
    float sunBlur = circleSDF(uv-sunBlurPos,radius);

    vec3 stripesColor = vec3(0.5,0.5,0.5);

    vec3 BGColor = mix(vec3(0.0+sin(iTime * sunSpeed),0.0+sin(iTime * sunSpeed),0.0),vec3(0.9,0.0,0.5),uv.y);
    vec4 BGLayer = vec4(BGColor,1.0);

    vec4 sunLayer = vec4(sunColor,1.0-step(radius,length(sunPos-uv)));
    vec4 sunBlurLayer = vec4(sunColor,1.0-smoothstep(-0.05,0.05,sunBlur));
    vec4 hillLayer = vec4(hillGrassColor,1.0-step(sin(uv.x*5.0+iTime)*0.2+0.2,uv.y));
    vec4 hill2Layer = vec4(hill2Color,1.0-step(sin(uv.x*5.0+iTime)*0.2+0.15,uv.y));
    vec4 stripesLayer = vec4(stripesColor,1.0-mod(floor(uv.x*8.0+iTime),4.0));

    vec4 allLayers = mix(BGLayer,sunLayer,sunLayer.a);
    allLayers = mix(allLayers,sunBlurLayer,sunBlurLayer.a);
    allLayers = mix(allLayers,stripesLayer,stripesLayer.a);
    allLayers = mix(allLayers,hillLayer,hillLayer.a);
    allLayers = mix(allLayers,hill2Layer,hill2Layer.a);
    fragColor=allLayers;
}
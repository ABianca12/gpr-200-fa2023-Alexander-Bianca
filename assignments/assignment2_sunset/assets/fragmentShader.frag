#version 450
out vec4 FragColor;
in vec2 UV;
uniform float iTime;
uniform vec3 BGColor;
uniform float sunSpeed;
uniform vec3 sunColor;
uniform float hillSpeed;
uniform vec3 hillGrassColor;
uniform float barsSpeed;
uniform vec3 barsColor;
uniform vec2 resolution;

float circleSDF(vec2 p, float r)
{
	return length(p) - r;
}

void main()
{
	//Global radius
    float radius = 0.1;
    float sunSpeed = 1.8;
    
    // Creating and defining initial sun variables
    vec2 sunPos = vec2(0.5, 0.5);
    sunPos.y *= (sin(iTime * sunSpeed));
    vec3 sunColor = vec3(1.0 + 0.5 * sin(iTime * sunSpeed),1.0 + 0.5 * cos(iTime * 1.8),0);
    
    // Creating and defining initial hillColor variables
    vec3 hillColor = hillGrassColor;
    vec3 hill2Color = vec3(0.9, 0.5, 0.0);
    
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = UV/resolution.xy;
    uv.y *= resolution.y / resolution.x;
    
    // Creating and defining initial sunBlur variables
    vec2 sunBlurPos = vec2(0.5, 0.5);
    sunBlurPos.y *= (sin(iTime * sunSpeed));
    float sunBlur = circleSDF(uv - sunBlurPos,radius);
    
    vec3 stripesColor = barsColor; 
    
    // BG gradient
    vec3 BGColor = mix(vec3(0.0 + sin(iTime * sunSpeed),0.0 + sin(iTime * sunSpeed),0.0),vec3(0.9,0.0,0.5),uv.y);
    
    // Creating Layers
    vec4 BGLayer = vec4(BGColor, 1.0);
    vec4 sunLayer = vec4(sunColor, 1.0 - step(radius, length(sunPos - uv)));
    vec4 sunBlurLayer = vec4(sunColor, 1.0 - smoothstep(-0.05,0.05,sunBlur));
    vec4 hillLayer = vec4(hillColor, 1.0 - step(sin(uv.x*5.0 + iTime) * 0.2 + 0.2,uv.y));
    vec4 hill2Layer = vec4(hill2Color, 1.0 - step(sin(uv.x*5.0 + iTime) * 0.2 + 0.15,uv.y));
    vec4 stripesLayer = vec4(stripesColor, 1.0 - mod(floor(uv.x*8.0 + iTime),4.0));
    
    // Mixing layers, layering technique from
    // https://www.shadertoy.com/view/ltXyD2
    vec4 allLayers = mix(BGLayer, sunLayer, sunLayer.a);
    allLayers = mix(allLayers, sunBlurLayer, sunBlurLayer.a);
    allLayers = mix(allLayers, stripesLayer, stripesLayer.a);
    allLayers = mix(allLayers, hillLayer, hillLayer.a);
    allLayers = mix(allLayers, hill2Layer, hill2Layer.a);
    
    // Output to screen
    FragColor = allLayers;
}
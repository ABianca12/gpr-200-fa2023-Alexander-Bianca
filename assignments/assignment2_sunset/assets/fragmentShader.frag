#version 450
out vec4 FragColor;
in vec3 Color;
in vec2 UV;

uniform vec3 _BGTopColor;
uniform vec3 _BGBottomColor;

uniform vec3 _SunRisingColor;
uniform vec3 _SunSettingColor;
uniform float _SunSpeed;

uniform vec3 _HillGrassColor;
uniform vec3 _Hill2Color;
uniform float _HillSpeed;

uniform vec3 _StripesColor;
uniform float _StripesSpeed;

uniform float _Time;
uniform vec2 _Resolution;

void main()
{
    // Sun variables
    vec2 sunPos = vec2(0.5, 0.5);
    float sunRange = 0.5;
    float sunRadius = 0.2;
    
    // Hill variables
    float hillFrequency = 10.0;
    float hillHeight = 0.4;
    float hillGrade = 0.08;
    
    float smoothing = 0.02;
    float brightness = 0.5;
    vec2 _UV = UV;

    float aspectRatio = _Resolution.x/_Resolution.y;
    _UV.x *= aspectRatio;
    
    // Sun objects
    sunPos.y = sin(_Time * _SunSpeed) * sunRange + sunPos.y;
    sunPos = vec2(sunPos.x * aspectRatio, sunPos.y);

    float sun = distance(_UV, sunPos);
    sun = smoothstep(sunRadius - smoothing, sunRadius + smoothing, sun);
    
    vec3 sunColor = mix(_SunSettingColor, _SunRisingColor, sunPos.y);
    
    // Hill objects
    float h = hillHeight + sin(_UV.x * hillFrequency + _HillSpeed * _Time) * hillGrade;
    h = smoothstep (h, h, _UV.y);
    float h2 = hillHeight - 0.05 + sin(_UV.x * hillFrequency + _HillSpeed * _Time) * hillGrade;
    h2 = smoothstep (h2, h2, _UV.y);
    
    vec3 hillGrassColor = _HillGrassColor * sunPos.y;
    vec3 hill2Color = _Hill2Color * sunPos.y;
    
    // Stripes objects
    float stripes = mod(floor(UV.x * 8.0 + _StripesSpeed * _Time), 2.0);
    stripes = smoothstep (stripes, stripes, _UV.y);

    vec3 stripesColor = _StripesColor * sunPos.y;

    // BG objects
    vec3 BGColor = mix(_BGBottomColor, _BGTopColor, _UV.y);
    BGColor *= sunPos.y + brightness;

    // Output to screen
    //vec3 color = mix(sunColor, BGColor, sun);
    vec3 color = mix(stripesColor, BGColor, stripes);
    color = mix(sunColor, color, sun);
    color = mix(hillGrassColor, color, h);
    color = mix(hill2Color, color, h2);
    FragColor = vec4(color,1.0);
}
#version 330 core

in vec4 clipSpace;
in vec2 textureCoords;

uniform vec3 color;

out vec4 fragColor;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;

uniform float moveFactor;

const float waveStrength = 0.002;

void main()
{
	vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 2.0 - 1.0) * waveStrength;
	vec2 distortion2 = (texture(dudvMap, vec2(-textureCoords.x + moveFactor, textureCoords.y + moveFactor)).rg * 2.0 - 1.0) * (waveStrength * 2);

	refractTexCoords += distortion1;
	//refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
	reflectTexCoords += distortion1 + distortion2;
	//reflectTexCoords.x = clamp(reflectTexCoords, 0.001, 0.999);
	//reflectTexCoords.y = clamp(reflectTexCoords, -0.999, -0.001);

	vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColor = texture(refractionTexture, refractTexCoords);

	//fragColor = vec4(color,1.0f);
	fragColor = mix(reflectColor, refractColor, 0.5);
}

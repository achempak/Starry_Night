#version 330 core

in vec3 fragNormal;
in vec2 pass_textureCoordinates;

out vec4 fragColor;

uniform sampler2D backgroundTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D blendMap;

uniform bool norm;

void main()
{
	vec3 norm_color = (normalize(fragNormal) + 1 )/ 2;
	fragColor = vec4(norm_color,1.0f);

	
	if(norm)
	{
		vec4 blendMapColor = texture(blendMap, pass_textureCoordinates);
		float backTextureAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
		vec2 tiledCoords = pass_textureCoordinates * 40.0;
		vec4 backgroundTextureColor = texture(backgroundTexture, tiledCoords) * backTextureAmount;
		vec4 rTextureColor = texture(rTexture, tiledCoords) * blendMapColor.r;
		vec4 gTextureColor = texture(gTexture, tiledCoords) * blendMapColor.g;
		vec4 bTextureColor = texture(bTexture, tiledCoords) * blendMapColor.b;

		vec4 totalColor = backgroundTextureColor + rTextureColor + gTextureColor + bTextureColor;

		fragColor = totalColor;
	}
}
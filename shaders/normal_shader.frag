#version 330 core

//uniform vec3 color;

in vec3 fragNormal;
out vec4 fragColor;

void main()
{
	vec3 norm_color = (normalize(fragNormal) + 1 )/ 2;
	fragColor = vec4(norm_color,1.0f);
}
#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const vec4 plane = vec4(0, -1, 0, 0);

void main()
{
	vec4 worldPosition = model * vec4(position, 1.0);

	gl_ClipDistance[0] = dot(worldPosition, plane);

	gl_Position = projection * view * worldPosition;
}
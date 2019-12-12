#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
//layout (location = 2) in vec2 textureCoordinates;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 plane;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out vec3 fragPos;
out vec3 fragNormal;
out vec2 pass_textureCoordinates;

void main()
{
	vec4 worldPos = model * vec4(position, 1.0);
	gl_ClipDistance[0] = dot(worldPos, plane);

	pass_textureCoordinates = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5) * 6.0;;

    fragPos = vec3(worldPos);
    fragNormal = mat3(transpose(inverse(model))) * normal;
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * view * model * vec4(position, 1.0);
}

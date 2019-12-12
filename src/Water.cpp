#include "Water.h"

Water::Water(glm::vec3 left, glm::vec3 width, glm::vec3 height, WaterFrameBuffer* f, std::string map) : ambient(glm::vec3(0,0,1)), height(left.y), ripple(0.0f)
{
	vertices.emplace_back(left);
	vertices.emplace_back(left + width);
	vertices.emplace_back(left + height);

	vertices.emplace_back(left + width);
	vertices.emplace_back(left + height + width);
	vertices.emplace_back(left + height);

	dudvTexture = new Texture(map);

	fbos = f;
	model = glm::mat4(1);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
		vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	std::cout << "Done initializing water." << std::endl;
}

Water::~Water()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	delete dudvTexture;
}

void Water::draw(glm::mat4 C, GLuint program)
{
	ripple += 0.002f;
	if (ripple > 1) ripple = 0.0f;

	glUseProgram(program);
	GLuint modelLoc = glGetUniformLocation(program, "model");
	GLuint color = glGetUniformLocation(program, "color");
	GLuint offset = glGetUniformLocation(program, "moveFactor");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(C * model));
	glUniform3fv(color, 1, glm::value_ptr(ambient));
	glUniform1f(offset, ripple);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}

float Water::getHeight()
{
	return height;
}

GLuint Water::getDudv()
{
	return dudvTexture->getId();
}
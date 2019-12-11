#include "Water.h"

Water::Water(glm::vec3 left, glm::vec3 width, glm::vec3 height) : ambient(glm::vec3(0,0,1))
{
	vertices.emplace_back(left);
	vertices.emplace_back(left + width);
	vertices.emplace_back(left + height);

	vertices.emplace_back(left + width);
	vertices.emplace_back(left + height + width);
	vertices.emplace_back(left + height);

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
}

void Water::draw(glm::mat4 C, GLuint program)
{
	glUseProgram(program);
	GLuint modelLoc = glGetUniformLocation(program, "model");
	GLuint color = glGetUniformLocation(program, "color");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(C * model));
	glUniform3fv(color, 1, glm::value_ptr(ambient));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}
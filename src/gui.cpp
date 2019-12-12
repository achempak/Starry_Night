#include "gui.h"

Gui::Gui(glm::vec3 left, glm::vec3 width, glm::vec3 height)
{
	vertices.emplace_back(left);
	vertices.emplace_back(left + width + height);
	vertices.emplace_back(left + height);

	vertices.emplace_back(left + width);
	vertices.emplace_back(left + height + width);
	vertices.emplace_back(left);

	std::vector<glm::vec2> texcoords;
	texcoords.emplace_back(glm::vec2(0.0, 0.0));
	texcoords.emplace_back(glm::vec2(1.0, 1.0));
	texcoords.emplace_back(glm::vec2(0.0, 1.0));
	texcoords.emplace_back(glm::vec2(1.0, 0.0));
	texcoords.emplace_back(glm::vec2(1.0, 1.0));
	texcoords.emplace_back(glm::vec2(0.0, 0.0));

	model = glm::mat4(1);
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbos);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
		vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texcoords.size(),
		texcoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	std::cout << "Done initializing gui." << std::endl;
}

Gui::~Gui()
{
	glDeleteBuffers(2, vbos);
	glDeleteVertexArrays(1, &vao);
}

void Gui::draw(glm::mat4 C, GLuint program)
{
	glUseProgram(program);
	GLuint modelLoc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(C * model));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}
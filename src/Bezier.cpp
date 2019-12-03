#include "Bezier.h"

Bezier::Bezier(std::vector<glm::vec3> &points, glm::vec3 col, std::string n, GLuint program) : color(col), name(n), prog(program)
{
	model = glm::mat4(1);
	num_points = points.size();
	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0.
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	colorLoc = glGetUniformLocation(prog, "color");
	modelLoc = glGetUniformLocation(prog, "model");

	std::cout << "Done initializing Bezier curve." << std::endl;
}

Bezier::Bezier(std::vector<glm::vec3>& points, glm::vec3 col, GLuint program) : Bezier::Bezier(points, col, "", program) {}

Bezier::~Bezier()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Bezier::draw(glm::mat4 C, GLuint program)
{
	glUseProgram(prog);
	glUniform3fv(colorLoc, 1, glm::value_ptr(color));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(C * model));
	glBindVertexArray(vao);
	glLineWidth(1.5f);
	glDrawArrays(GL_LINE_STRIP, 0, num_points);
	glBindVertexArray(0);
}

void Bezier::update_points(std::vector<glm::vec3> points)
{
	glUseProgram(prog);
	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Bezier::update_color(glm::vec3 col)
{
	color = col;
}

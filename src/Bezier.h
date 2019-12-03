#ifndef BEZIER_H
#define BEZIER_H

#include "Node.hpp"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class Bezier : public Node
{
private:
	glm::mat4 model;
	GLuint vao;
	GLuint vbo;
	glm::vec3 color;
	std::string name;
	GLuint prog, colorLoc, modelLoc;
	int num_points;
	
public:
	Bezier(std::vector<glm::vec3>& points, glm::vec3 col, std::string name, GLuint program);
	Bezier(std::vector<glm::vec3>& points, glm::vec3 col, GLuint program);
	~Bezier();
	virtual void draw(glm::mat4 C, GLuint program);
	virtual void update(glm::mat4 C) {};
	virtual void updatePos(glm::mat4 C) {};
	virtual std::string getName()
	{
		return name;
	};
	GLuint getProgram() { return prog; };
	void update_points(std::vector<glm::vec3> points);
	void update_color(glm::vec3 col);
};

#endif // !BEZIER_H


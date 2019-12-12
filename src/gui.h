#ifndef GUI_H
#define GUI_H

#include "Node.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class Gui : public Node

{
private:
	std::vector<glm::vec3> vertices;
	glm::mat4 model;
	GLuint vao;
	GLuint vbos[2];

public:
	Gui(glm::vec3 left, glm::vec3 width, glm::vec3 height);
	~Gui();
	virtual void draw(glm::mat4 C, GLuint program);
	virtual void update(glm::mat4 C) {};
	virtual void updatePos(glm::mat4 C) {};
	virtual std::string getName() {
		return "Gui";
	}
};

#endif // !GUI_H

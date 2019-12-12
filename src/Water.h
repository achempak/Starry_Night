#ifndef WATER_H
#define WATER_H

#include "Node.hpp"
#include "WaterFrameBuffer.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class Water : public Node

{
private:
	std::vector<glm::vec3> vertices;
	glm::mat4 model;
	GLuint vao;
	GLuint vbo;
	WaterFrameBuffer* fbos;
	Texture* dudvTexture;

	glm::vec3 ambient;
	float height; // Height of water above/below 0
	float ripple; //For ripples

public:
	Water(glm::vec3 left, glm::vec3 width, glm::vec3 height, WaterFrameBuffer* f, std::string map);
	~Water();
	virtual void draw(glm::mat4 C, GLuint program);
	virtual void update(glm::mat4 C) {};
	virtual void updatePos(glm::mat4 C) {};
	virtual std::string getName() {
		return "Water";
	}
	float getHeight();
	GLuint getDudv();
};

#endif // !WATER_H

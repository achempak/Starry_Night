#ifndef Environment_h
#define Environment_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

class Environment
{
private:
	std::vector<GLfloat> skyboxVertices;
	GLuint vao, vbo;
	GLuint textureID;
	glm::mat4 model;
public:
	Environment(std::vector<std::string> &faces);
	void draw(glm::mat4 C, GLuint program);
	GLuint getTextureID() { return textureID; };
};

#endif /* Environment_hpp */

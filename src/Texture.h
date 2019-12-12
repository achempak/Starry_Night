#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <string>

class Texture
{
private:
	GLuint textureId;

public:
	Texture(std::string filename);
	~Texture();
	GLuint getId();
};

#endif // !TEXTURE_H
#include "Texture.h"

Texture::Texture(std::string filename)
{
	glGenTextures(1, &textureId); // Get unique ID for texture
	glBindTexture(GL_TEXTURE_2D, textureId); // Tell OpenGL which texture to edit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set bi-linear interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // for both filtering modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture edge mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	// Depending on the image library, the texture image may have to be flipped vertically
	// Load image into OpenGL texture in GPU memory:
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, // Always GL_TEXTURE_2D for image textures
			0, // 0 for now
			GL_RGB, // Format OpenGL uses for image without alpha channel
			width, height, // Width and height
			0, // The border of the image
			GL_RGB, // GL_RGB, because pixels are stored in RGB format
			GL_UNSIGNED_BYTE, // GL_UNSIGNED_BYTE, because pixels are stored as unsigned numbers
			data); // The actual RGB image data
		stbi_image_free(data);
		std::cout << "Texture successfully loaded at path: " << filename << std::endl;
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureId);
}

GLuint Texture::getId()
{
	return textureId;
}
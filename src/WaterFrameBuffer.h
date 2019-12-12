#ifndef WATERFRAMEBUFFER_H
#define WATERFRAMEBUFFER_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <iostream>

class WaterFrameBuffer
{
private:
	static const int REFLECTION_WIDTH = 320;
	static const int REFLECTION_HEIGHT = 180;
	static const int REFRACTION_WIDTH = 1280;
	static const int REFRACTION_HEIGHT = 720;

	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;
	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

public:
	WaterFrameBuffer();
	~WaterFrameBuffer();
	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void unbindCurrentFrameBuffer(int width, int height);
	GLuint getReflectionTexture();
	GLuint getRefractionTexture();
	GLuint getRefractionDepthTexture();
	void initializeReflectionFrameBuffer();
	void initializeRefractionFrameBuffer();
	void bindFrameBuffer(GLuint frameBuffer, int width, int height);
	GLuint createFrameBuffer();
	GLuint createTextureAttachment(int width, int height);
	GLuint createDepthTextureAttachment(int width, int height);
	GLuint createDepthBufferAttachment(int width, int height);
};

#endif // !WATERFRAMEBUFFER_H


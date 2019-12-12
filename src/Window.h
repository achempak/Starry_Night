#ifndef _WINDOW_H_
#define _WINDOW_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <vector>
#include <memory>

#include "shader.h"
#include "Node.hpp"
#include "Transform.hpp"
#include "Geometry.hpp"
#include "Environment.h"
#include "Track.h"
#include "RGeometry.h"
#include "Terrain.h"
#include "Water.h"
#include "WaterFrameBuffer.h"
#include "gui.h"

class Window
{
public:
	static int width;
	static int height;
	static double fovy;
	static const char* windowTitle;
	static double x_prev;
	static double y_prev;
	static Geometry* sphere_geo;
    static bool boundingOn;
	static bool isEnergy;
	static bool toggleCam;
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eye, center, up;
	static GLuint program, projectionLoc, viewLoc, colorLoc, norm_factor;
	static GLuint env_program, skyboxLoc, skyProjectionLoc, skyViewLoc;
	static GLuint simple_program, simpleProjectionLoc, simpleViewLoc, simpleColorLoc, simplePlaneLoc;
	static GLuint water_program, waterProjectionLoc, waterViewLoc, waterColorLoc, waterReflectionLoc, waterRefractionLoc, waterDudvLoc;
	static GLuint gui_program, guiProjectionLoc, guiViewLoc, guiTextureLoc;
    static std::vector<glm::vec4> frustrum_planes;
	static Environment* sky;
	static Transform* sphere;
	static Terrain* terrain;
	static Water* water;
    
    // Normal coloring mode
    static bool normal_coloring;
    
    // Keep track of how rotation works
    static bool holding;

	// Zoom mode (FOV or forward/backward)
	static bool zoomMode;

	// Keep track of moving (wasd)
	static bool key_held;
	static char movement_dir;
    
    // Lighting Info
    static GLuint lightLoc, lightAmb, lightDiff, lightSpec, lightLin;
    static glm::vec3 lightPos;
    static glm::vec3 lightAmbient;
    static glm::vec3 lightDiffuse;
    static glm::vec3 lightSpecular;
    static float linear;
    
    // Material Info
    static GLuint matAmb, matDiff, matSpec, shine;
    static glm::vec3 matAmbient;
    static glm::vec3 matDiffuse;
    static glm::vec3 matSpecular;
    static float shininess;

	// Water Stuff
	static WaterFrameBuffer* fbos;

	// GUI stuff
	static Gui* gui;

	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void idleCallback(GLFWwindow* window);
	static void displayCallback(GLFWwindow*);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void furstrum_calculation();
    static void calc_plane(std::vector<glm::vec4> &planes, glm::vec3 normal, glm::vec3 point, int plane_number);
    static void toggleBounding(bool on);
	static void move(char c);
	static void renderScene();
};

#endif

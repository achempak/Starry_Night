#include "Window.h"

int Window::width;
int Window::height;
double Window::x_prev;
double Window::y_prev;
double Window::fovy = 60.0;
glm::vec3 Window::lightPos;
bool Window::holding = false;
bool Window::key_held = false;
char Window::movement_dir = 'w';
bool Window::normal_coloring = false;
bool Window::boundingOn = true;
bool Window::zoomMode = true;
bool Window::isEnergy = false;
bool Window::toggleCam = false;
std::vector<glm::vec4> Window::frustrum_planes(6, glm::vec4(0));

const char* Window::windowTitle = "Final";

Geometry* Window::sphere_geo;
Transform* Window::sphere;
Terrain* Window::terrain;
Water* Window::water;
Gui* Window::gui;

glm::mat4 Window::projection;  // Projection matrix.

glm::vec3 Window::eye(0, 0, 0); // Camera position.
glm::vec3 Window::center(0, 0, 20); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

// Skybox
Environment* Window::sky;

// Lighting color details
glm::vec3 light_details(1.0f, 0.6f, 0.4f);
glm::vec3 Window::lightAmbient = light_details*glm::vec3(0.2);
glm::vec3 Window::lightDiffuse = light_details*glm::vec3(0.6);
glm::vec3 Window::lightSpecular = light_details*glm::vec3(1.0);
float Window::linear = 0.02;

GLuint Window::program; // The shader program id.
GLuint Window::env_program; // Skybox program id.
GLuint Window::simple_program;
GLuint Window::water_program;
GLuint Window::gui_program;

GLuint Window::projectionLoc; // Location of projection in shader.
GLuint Window::viewLoc; // Location of view in shader.
GLuint Window::colorLoc; // Location of color in shader.

GLuint Window::simpleColorLoc;
GLuint Window::simpleProjectionLoc;
GLuint Window::simpleViewLoc;
GLuint Window::simplePlaneLoc;

GLuint Window::skyProjectionLoc;
GLuint Window::skyViewLoc;
GLuint Window::skyboxLoc; // Location of skybox in shader.

// Water stuff
GLuint Window::waterColorLoc;
GLuint Window::waterProjectionLoc;
GLuint Window::waterViewLoc;
GLuint Window::waterReflectionLoc;
GLuint Window::waterRefractionLoc;
GLuint Window::waterDudvLoc;
WaterFrameBuffer* Window::fbos;

// GUI stuff
GLuint Window::guiProjectionLoc;
GLuint Window::guiViewLoc;
GLuint Window::guiTextureLoc;

GLuint Window::lightLoc;
GLuint Window::lightAmb;
GLuint Window::lightSpec;
GLuint Window::lightDiff;
GLuint Window::lightLin;
GLuint Window::matAmb;
GLuint Window::matDiff;
GLuint Window::matSpec;
GLuint Window::shine;
GLuint Window::norm_factor;


bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
#ifdef __APPLE__
	program = LoadShaders("../shaders/shader.vert", "../shaders/shader.frag");
	env_program = LoadShaders("../shaders/env_shader.vert", "../shaders/env_shader.frag");
	simple_program = LoadShaders("../shaders/shader.vert", "../shaders/simple_shader.frag");
	water_program = LoadShaders("../shaders/water_shader.vert", "../shaders/water_shader.frag");
	gui_program = LoadShaders("../shaders/gui_shader.vert", "../shaders/gui_shader.frag");
#else
	program = LoadShaders("../shaders/shader.vert", "../shaders/shader.frag");
	env_program = LoadShaders("../shaders/env_shader.vert", "../shaders/env_shader.frag");
	simple_program = LoadShaders("../shaders/shader.vert", "../shaders/normal_shader.frag");
	water_program = LoadShaders("../shaders/water_shader.vert", "../shaders/water_shader.frag");
	gui_program = LoadShaders("../shaders/gui_shader.vert", "../shaders/gui_shader.frag");

#endif

	// Check the shader program.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	else if (!env_program)
	{
		std::cerr << "Failed to initialize skybox shader program" << std::endl;
	}

	else if (!simple_program)
	{
		std::cerr << "Failed to initialize simple shader program" << std::endl;
	}

	else if (!water_program)
	{
		std::cerr << "Failed to initialize water shader program" << std::endl;
	}

	else if (!gui_program)
	{
		std::cerr << "Failed to initialize gui shader program" << std::endl;
	}

	//// Activate the shader program.
	//glUseProgram(program);
	// Get the locations of uniform variables.
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	colorLoc = glGetUniformLocation(program, "color");
    lightLoc = glGetUniformLocation(program, "light.position");
    lightAmb = glGetUniformLocation(program, "light.ambient");
    lightDiff = glGetUniformLocation(program, "light.diffuse");
    lightSpec = glGetUniformLocation(program, "light.specular");
    lightLin = glGetUniformLocation(program, "light.linear");
    norm_factor = glGetUniformLocation(program, "norm_factor");

	// Skybox shader locs
	skyProjectionLoc = glGetUniformLocation(env_program, "projection");
	skyViewLoc = glGetUniformLocation(env_program, "view");
	skyboxLoc = glGetUniformLocation(env_program, "skybox");

	// Simple shader locs
	simpleProjectionLoc = glGetUniformLocation(simple_program, "projection");
	simpleViewLoc = glGetUniformLocation(simple_program, "view");
	simplePlaneLoc = glGetUniformLocation(simple_program, "plane");

	// Water shader locs
	waterProjectionLoc = glGetUniformLocation(water_program, "projection");
	waterViewLoc = glGetUniformLocation(water_program, "view");
	waterReflectionLoc = glGetUniformLocation(water_program, "reflectionTexture");
	waterRefractionLoc = glGetUniformLocation(water_program, "refractionTexture");
	waterDudvLoc = glGetUniformLocation(water_program, "dudvMap");

	// GUI shader locs
	guiProjectionLoc = glGetUniformLocation(gui_program, "projection");
	guiViewLoc = glGetUniformLocation(gui_program, "view");
	guiTextureLoc = glGetUniformLocation(gui_program, "myTextureSampler");

	return true;
}

bool Window::initializeObjects()
{
	// Setup water FBOs
	fbos = new WaterFrameBuffer();

	//sphere_geo = new Geometry("../resources/sphere.obj", "sphere_geo");
	//sphere_geo->setMaterial(glm::vec3(0, 0.5, 1));

	std::vector<std::string> faces = {
		"../resources/mp_tf/thefog_lf.tga",
		"../resources/mp_tf/thefog_rt.tga",
		"../resources/mp_tf/thefog_up.tga",
		"../resources/mp_tf/thefog_dn.tga",
		"../resources/mp_tf/thefog_ft.tga",
		"../resources/mp_tf/thefog_bk.tga",
	};
	sky = new Environment(faces);
	GLuint skyTexture = sky->getTextureID();

	std::vector<float> corners = { 0.0f, 10.0f, 5.0f, 3.0f };
	terrain = new Terrain(10, corners);

	//std::vector<std::vector<float>> heightmap = terrain->getHeight();
	int map_width = 250; // Since "grid_size" is 500
	std::string dudv_path = "../resources/waterDUDV.png";
	//water = new Water(glm::vec3(0, terrain->getAvgHeight(), 0), glm::vec3(500, 0, 0), glm::vec3(0, 0, 500), fbos, dudv_path);
	water = new Water(glm::vec3(0, 0, 0), glm::vec3(500, 0, 0), glm::vec3(0, 0, 500), fbos, dudv_path);

	gui = new Gui(glm::vec3(0, 0, 0), glm::vec3(10, 0, 0), glm::vec3(0, 10, 0));

	std::cout << "dudv GLuint: " << water->getDudv() << std::endl;
	std::cout << "reflection GLuint: " << fbos->getReflectionTexture() << std::endl;
	std::cout << "refraction GLuint: " << fbos->getRefractionTexture() << std::endl;

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete sky;
	delete terrain;
	delete water;
	delete fbos;
	delete gui;

	// Delete the shader program.
	glDeleteProgram(program);
	glDeleteProgram(env_program);
	glDeleteProgram(simple_program);
	glDeleteProgram(water_program);
	glDeleteProgram(gui_program);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
    Window::width = width;
    Window::height = height;
	Window::projection = glm::perspective(glm::radians(fovy),
		double(width) / (double)height, 1.0, 1000.0);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	Window::projection = glm::perspective(glm::radians(fovy), 
		double(width) / (double)height, 1.0, 1000.0);
}

void Window::idleCallback(GLFWwindow* window)
{   
    // Deal with rotating the camera here.
    if(holding)
    {
        double x_curr, y_curr;
        glfwGetCursorPos(window, &x_curr, &y_curr);
		if (x_curr > width - 0.001) x_curr = width;
		else if (x_curr < 0.001) x_curr = 0;
		if (y_curr > height - 0.001) y_curr = height;
		else if (y_curr < 0.001) y_curr = 0;
        double mouse_travel = (x_curr-x_prev)*(x_curr-x_prev) + (y_curr-y_prev)*(y_curr-y_prev);
        if(mouse_travel > 1)
        {
            /*float width = 640;
            float height = 480;*/
            float r_square = width * width / 4 + height * height / 4;

            float x_o = (float)x_prev - width / 2;
            float y_o = -((float)y_prev - height / 2); // Taking negative since bottom left is (0,0)
            float x_n = (float)x_curr - width / 2;
            float y_n = -((float)y_curr - height / 2);
            float z_o = (float)sqrt(r_square - x_o * x_o - y_o * y_o);
            float z_n = (float)sqrt(r_square - x_n * x_n - y_n * y_n);

            std::cout << "x_old, y_old, z_old: " << x_o << ", " << y_o << ", " << z_o << std::endl;
            std::cout << "x_new, y_new, z_new: " << x_n << "," << y_n << "," << z_n << std::endl;

            glm::vec3 oldPos(x_o, y_o, z_o);
            glm::vec3 newPos(x_n, y_n, z_n);
            oldPos = glm::normalize(oldPos);
            newPos = glm::normalize(newPos);
            glm::vec3 axis = glm::cross(oldPos, newPos);
            float angle = glm::acos(glm::dot(oldPos, newPos));

            std::cout << "Axis: (";
            for (int i = 0; i < 3; ++i)
            {
                std::cout << axis[i] << ",";
            }
            std::cout << ")" << std::endl;
            std::cout << "Angle: " << angle << std::endl;

			std::cout << "Current Center point: (";
			for (int i = 0; i < 3; ++i)
			{
				std::cout << center[i] << ",";
			}
			std::cout << ")" << std::endl;

            glm::vec3 forward = center - eye;
			glm::vec3 newForward = glm::rotate(view * glm::vec4(forward, 0), angle, -axis);
			newForward = glm::vec3(glm::inverse(view) * glm::vec4(newForward, 0));
            center = newForward + eye;
            view = glm::lookAt(eye, center, up);
            
            x_prev = x_curr;
            y_prev = y_curr;
        }
    }
	//if (toggleCam)
	//{
	//	glm::vec3 temp_eye = track->get_ball_point();
	//	glm::vec3 temp_center = (track->get_tangent());
	//	view = glm::lookAt(temp_eye, temp_center, Window::up);
	//}
	if (key_held)
	{
		move(movement_dir);
	}
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fbos->bindReflectionFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fbos->bindRefractionFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
	// Specify the values of the uniform variables we are going to use.
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(lightAmb, 1, glm::value_ptr(lightAmbient));
    glUniform3fv(lightSpec, 1, glm::value_ptr(lightSpecular));
    glUniform3fv(lightDiff, 1, glm::value_ptr(lightDiffuse));
    glUniform1f(lightLin, linear);
    if(normal_coloring)
        glUniform1f(norm_factor, 1.0f);
    else
        glUniform1f(norm_factor, 0.0f);

    glUseProgram(env_program);
	// Skybox uniform variables
	glUniformMatrix4fv(skyProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	//glm::mat4 temp = glm::mat4(glm::mat3(view)); // Remove translation from the view matrix
	glUniformMatrix4fv(skyViewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Simple uniform variables
	glUseProgram(simple_program);
	glUniformMatrix4fv(simpleProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(simpleViewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Water uniform variables
	glUseProgram(water_program);
	glUniformMatrix4fv(waterProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(waterViewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniform1i(waterReflectionLoc, 0);
	glUniform1i(waterRefractionLoc, 1);
	glUniform1i(waterDudvLoc, 2);

	// Gui uniform variables
	glUseProgram(gui_program);
	glUniformMatrix4fv(guiProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(guiViewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniform1i(guiTextureLoc, 0);

	// Enable clipping
	glEnable(GL_CLIP_DISTANCE0);
	
	// First, render scene without water to reflection framebuffer
	fbos->bindReflectionFrameBuffer();
	//float distance = 2 * (eye.y - water->getHeight());
	//float angle = glm::acos(glm::dot(glm::vec3(0, 1, 0), glm::normalize(center - eye)));
	glm::mat4 reflect = glm::mat4(1);
	reflect[1][1] = -1;
	//view = glm::lookAt(eye-glm::vec3(0,distance,0), center, up); // Move camera eye below water's surface
	glm::mat4 oldview = view;
	view = reflect * view;
	glUseProgram(simple_program);
	glUniform4fv(simplePlaneLoc, 1, glm::value_ptr(glm::vec4(0, 1, 0, -water->getHeight())));
	sky->draw(glm::scale(glm::vec3(500, 500, 500)), env_program);
	terrain->draw(glm::mat4(1), simple_program);
	//view = glm::lookAt(eye, center, up); // Move camera back
	view = oldview;

	// Second, render scene without water to refraction framebuffer
	fbos->bindRefractionFrameBuffer();
	glUseProgram(simple_program);
	glUniform4fv(simplePlaneLoc, 1, glm::value_ptr(glm::vec4(0, -1, 0, water->getHeight())));
	sky->draw(glm::scale(glm::vec3(500, 500, 500)), env_program);
	terrain->draw(glm::mat4(1), simple_program);
	fbos->unbindCurrentFrameBuffer(Window::width, Window::height);

	// Finally render full scene
	glDisable(GL_CLIP_DISTANCE0);
	sky->draw(glm::scale(glm::vec3(500, 500, 500)), env_program);
	terrain->draw(glm::mat4(1), simple_program);

	glUseProgram(gui_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, water->getDudv());
	gui->draw(glm::mat4(1), gui_program);

	glUseProgram(water_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbos->getReflectionTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbos->getRefractionTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, water->getDudv());

	water->draw(glm::mat4(1), water_program);
    
    // Render the light.
//    model = sphere->getModel();
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glUniform3fv(matDiff, 1, glm::value_ptr(glm::vec3(0)));
//    glUniform3fv(matSpec, 1, glm::value_ptr(glm::vec3(0)));
//    glUniform3fv(matAmb, 1, glm::value_ptr(lightAmbient*2.0f));
//    sphere->draw();

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float delta = 0.3f;
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_F:
			terrain->wireframe(!(terrain->getWireframe()));
			break;
        case GLFW_KEY_B:
            toggleBounding(!boundingOn);
            boundingOn = !boundingOn;
            break;
		case GLFW_KEY_C:
			toggleCam = !toggleCam;
			if (!toggleCam)
			{
				view = glm::lookAt(Window::eye, Window::center, Window::up);
			}
        case GLFW_KEY_N:
            normal_coloring = !normal_coloring;
            break;
		case GLFW_KEY_I:
			zoomMode = !zoomMode;
			break;
		case GLFW_KEY_W:
			movement_dir = 'w';
			key_held = true;
			break;
		case GLFW_KEY_S:
			movement_dir = 's';
			key_held = true;
			break;
		case GLFW_KEY_A:
			movement_dir = 'a';
			key_held = true;
			break;
		case GLFW_KEY_D:
			movement_dir = 'd';
			key_held = true;
			break;
		default:
			break;
		}
	}
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			if (movement_dir == 'w') key_held = false;
			break;
		case GLFW_KEY_S:
			if (movement_dir == 's') key_held = false;
			break;
		case GLFW_KEY_A:
			if (movement_dir == 'a') key_held = false;
			break;
		case GLFW_KEY_D:
			if (movement_dir == 'd') key_held = false;
			break;
		default:
			break;
		}
	}
}

void Window::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (!zoomMode)
	{
		// Zoom in and out of scene
		glm::vec3 forward = center - eye;
		std::cout << "Zoom: " << glm::length(forward) << std::endl;
		glm::vec3 new_forward = forward * (float)(1 + yoffset / 10);
		eye = center - new_forward;
		view = glm::lookAt(eye, center, up);
	}

	else
	{
		//Change FOV
		Window::fovy += yoffset / 2;
		std::cout << "FOV: " << Window::fovy << std::endl;
		Window::projection = glm::perspective(glm::radians(Window::fovy),
			double(width) / (double)height, 1.0, 1000.0);
	}

}
//
void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		glfwGetCursorPos(window, &x_prev, &y_prev);

		if (action == GLFW_PRESS)
		{
			holding = true;
		}
		else if (action == GLFW_RELEASE)
		{
			holding = false;
			std::cout << "NOT HOLD" << std::endl;
		}
	}
}

void Window::furstrum_calculation()
{
    float nearDist = 0;
    float farDist = 100.0f;
    float fov = glm::radians(Window::fovy);
    float hnear = 2 * glm::tan(fov/2) * nearDist;
    float wnear = hnear * (float) ((double) width/ (double) height);
    float hfar = 2 * glm::tan(fov/2) * farDist;
    float wfar = hfar * (float) ((double) width/ (double) height);
    
    
    glm::vec3 d = center - eye;
    d = glm::normalize(d);
    glm::vec3 right =glm::cross(d, up);
    right = glm::normalize(right);
    glm::vec3 fc = eye + d * farDist;
//    ftl = fc + (up * hfar/2) - (right * wfar/2);
//    ftr = ftl + 2 * right * wfar/2;
//    fbl = ftl - 2 * up * hfar/2;
//    fbr = ftr - 2 * up * hfar/2;
//
    glm::vec3 nc = eye + d * nearDist;
//    ntl = nc + (up * nfar/2) - (right * nfar/2);
//    ntr = ntl + 2 * right * nfar/2;
//    nbl = ntl - 2 * up * nfar/2;
//    nbr = ntr - 2 * up * nfar/2;
    
    glm::vec3 a = (nc + right * wnear / 2.0f) - eye;
    glm::vec3 a_norm = glm::normalize(a);
    glm::vec3 normalRight = glm::cross(a_norm, up);
    glm::vec3 b = (nc - right * wnear / 2.0f) - eye;
    glm::vec3 b_norm = glm::normalize(b);
    glm::vec3 normalLeft = glm::cross(up, b_norm);
    glm::vec3 c = (nc + up * hnear/2.0f) - eye;
    glm::vec3 c_norm = glm::normalize(c);
    glm::vec3 normalTop = glm::cross(right, c_norm);
    glm::vec3 e = (nc - up * hnear/2.0f) - eye;
    glm::vec3 e_norm = glm::normalize(e);
    glm::vec3 normalDown = glm::cross(e_norm, right);
    
    calc_plane(frustrum_planes, -d, nc, 0); // Near plane
    calc_plane(frustrum_planes, d, fc, 1); // Far plane
    calc_plane(frustrum_planes, normalRight, a, 2); // Right plane
    calc_plane(frustrum_planes, normalLeft, b, 3); // Left plane
    calc_plane(frustrum_planes, normalTop, c, 4); // Top plane
    calc_plane(frustrum_planes, normalDown, e, 5); // Bottom plane
}

/**
 * Determines the parameters of a plane (a, b, c, and d) given a normal and a point.
 */
void Window::calc_plane(std::vector<glm::vec4> &planes, glm::vec3 normal, glm::vec3 point, int plane_number)
{
    glm::vec4 params = glm::vec4(normal, 0.0f);
    params.w = -normal.x*point.x - normal.y*point.y - normal.z*point.z;
    planes[plane_number] = params;
}

void Window::toggleBounding(bool on)
{
    sphere_geo->toggle_bounding(on);
}

void Window::move(char c)
{
	float movement_speed = 1.001f;
	if (c == 'w')
	{
		glm::vec3 forward = center - eye;
		glm::vec3 norm_forward = glm::normalize(forward);
		eye = eye + movement_speed * norm_forward;
		center = center + movement_speed * norm_forward;
		view = glm::lookAt(eye, center, up);
	}
	if (c == 's')
	{
		glm::vec3 forward = center - eye;
		glm::vec3 norm_forward = glm::normalize(forward);
		eye = eye - movement_speed * norm_forward;
		center = center - movement_speed * norm_forward;
		view = glm::lookAt(eye, center, up);
	}
	if (c == 'a')
	{
		glm::vec3 forward = center - eye;
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		eye = eye - movement_speed * right;
		center = center - movement_speed * right;
		view = glm::lookAt(eye, center, up);
	}
	if (c == 'd')
	{
		glm::vec3 forward = center - eye;
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		eye = eye + movement_speed * right;
		center = center + movement_speed * right;
		view = glm::lookAt(eye, center, up);
	}
}

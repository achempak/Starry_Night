#ifndef TRACK_H
#define TRACK_H

#ifdef _WIN32
	#include <GL/glew.h>
#endif
#include "glfw/glfw3.h"
#include "Geometry.hpp"
#include "Transform.hpp"
#include "Bezier.h"
#include "RGeometry.h"

class Track
{
private:
	std::vector<glm::vec3> all_points; //Contains all 24 control points
	std::vector<glm::vec3> all_curve_points; // Contains all points on the curve
    glm::vec3 ball_point; // Current point the ball is on
    int closest_index; // Closest index of point on curve (of the 150 calculated pts) to the ball.
	Transform* base; //Contains 8 Bezier curves, 24 spheres (control points), 23 line segments
	Bezier* track_curve;
    Transform* ball;
	Transform* sel_point; //Curren=tly selected point

    std::vector<glm::mat4> C; // Coeffs for Bezier curve
	int curr_point = 1; // Currently selected point
	int curr_handle = 0; // Currently handle
	std::vector<Transform*> handle_points; // Don't need this. Could get handles through base. This is easier.
	std::vector<Transform*> interp_points; // Don't need this either. Makes life easier.
	std::vector<Bezier*> handle_lines;
    
	bool isPaused;
	bool isEnergy;
	float max_height;
	float velocity;
	float prev_time;

	void set_max_height();
public:
	Track(std::vector<glm::vec3> input_points, Geometry* sphere_geo,  Geometry* spehre_geo2, Geometry* sphere_geo3, RGeometry* rsphere, GLuint bez_program);
	~Track();
	void draw(glm::mat4 C, GLuint program);
	void toggle_point(bool move_right);
	void move_point(glm::vec3 displacement);
    void update_pos();
	void pause();
	void energy_on();
	glm::vec3 get_tangent();
	glm::vec3 get_ball_point();
	static void calc_curve(std::vector<glm::vec3> control_points, std::vector<glm::vec3> &curve_points, std::vector<glm::mat4> &coeffs);
    static void get_next_point(std::vector<glm::vec3> curve_points, glm::vec3 &point, int &index, float speed, std::vector<glm::mat4> C);
    static void print_point(glm::vec3 &point);
	static void print_mat(glm::mat4& mat);
};

#endif // !TRACK_H

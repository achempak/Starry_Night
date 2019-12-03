#include "Track.h"

// input_points is a set of 8 interpolating points
// all_points is a set of all control points in order (interp. pt, handle1, handle2, interp., ...)
Track::Track(std::vector<glm::vec3> input_points, Geometry* sphere_geo, Geometry* sphere_geo2, Geometry* sphere_geo3, RGeometry* rsphere, GLuint bez_program)
{
	base = new Transform(glm::mat4(1));
	Transform* track = new Transform(glm::mat4(1), "track");
	base->addChild(track);
    closest_index = 1; //Ball starts on index 0. So closest index greater than this is 1.
	velocity = 0.01f;
	isPaused = false;
	isEnergy = false;
	max_height = 0.0f;
	prev_time = glfwGetTime();

	// First determine handle control points.
	int counter = 0;
	for (int i = 0; i < 24; ++i)
	{
		// These are interpolating points
		if (i % 3 == 0)
		{
			all_points.emplace_back(input_points[counter++]);
			Transform* interp = new Transform(glm::translate(all_points[i]) * glm::scale(glm::vec3(0.01)), "interpolating point");
			interp_points.emplace_back(interp);
			interp->addChild(dynamic_cast<Node*>(sphere_geo2));
			track->addChild(interp);
		}
		// These are handle points right before interpolating points
		else if ((i - 2) % 3 == 0)
		{
			glm::vec3 handle1 = input_points[counter - 1] + glm::vec3{ 1.0f, 1.0f, 1.0f };
			all_points.emplace_back(handle1);
		}
		// Temporarily input vector of 0s for handle points right after interpolating points
		else
		{
			all_points.emplace_back(glm::vec3(0));
		}
	}
	all_points[1] = 2.0f * all_points[0] - all_points[23];
	for (int i = 1; i < 8; ++i)
	{
		all_points[i * 3 + 1] = 2.0f * all_points[i * 3] - all_points[i * 3 - 1];
	}

	sel_point = new Transform(glm::translate(all_points[1]) * glm::scale(glm::vec3(0.015)), "selected_point");
	sel_point->addChild(dynamic_cast<Node*>(sphere_geo3));
	track->addChild(dynamic_cast<Node*>(sel_point));
    // Print the control points
    for(int i = 0; i < all_points.size(); ++i)
    {
        print_point(all_points[i]);
    }

	// We now have all the control points.
	// Time to calculate points on curve.
	std::cout << "Size of all_points: " << all_points.size() << std::endl;
	for (int i = 0; i < 7; ++i)
	{
        calc_curve(std::vector<glm::vec3>{all_points[3*i], all_points[3*i+1], all_points[3*i+2], all_points[3*i+3]}, all_curve_points, C);
	}
	std::vector<glm::vec3> last_seg = { all_points[21], all_points[22], all_points[23], all_points[0] };
	calc_curve(last_seg, all_curve_points, C);
	std::cout << "Initial C: " << std::endl;
	for (auto iter = C.begin(); iter != C.end(); ++iter)
	{
		print_mat(*iter);
	}
    ball_point = all_curve_points[0];
	set_max_height();
	std::cout << "Size of all_curve_points: " << all_curve_points.size() << std::endl;

	track_curve = new Bezier(all_curve_points, glm::vec3{ 1, 0, 0 }, "track_curve", bez_program);
	track->addChild(dynamic_cast<Node*>(track_curve));
    
    ball = new Transform(glm::translate(ball_point)*glm::scale(glm::vec3(0.03)), "ball");
    ball->addChild(dynamic_cast<Node*>(rsphere));
    track->addChild(ball);

	for (int i = 0; i < 8; ++i)
	{
		Transform* handle = new Transform(glm::mat4(1), "handle" + i);
		Transform* handle1 = new Transform(glm::translate(all_points[3 * i + 1]) * glm::scale(glm::vec3(0.01)), "handle_holder"+i*2);
		Transform* handle2 = new Transform(glm::translate(all_points[3 * i + 2]) * glm::scale(glm::vec3(0.01)), "handle_holder"+i*2+1);
		handle_points.emplace_back(handle1);
		handle_points.emplace_back(handle2);
		Transform* handle_line_trans = new Transform(glm::mat4(1));
		Bezier* handle_line;
		if (i == 0)
		{
            std::vector<glm::vec3> pts = {all_points[1], all_points[23]};
			handle_line = new Bezier(pts, glm::vec3{ 0, 1, 0 }, "handle_line" + i, bez_program);
		}
		else
		{
            std::vector<glm::vec3> pts = {all_points[3 * i - 1], all_points[3 * i + 1]};
			handle_line = new Bezier(pts, glm::vec3{ 1, 1, 0 }, "handle_line" + i, bez_program);
		}
		handle_lines.emplace_back(handle_line);
		handle1->addChild(dynamic_cast<Node*>(sphere_geo));
		handle2->addChild(dynamic_cast<Node*>(sphere_geo));
		handle->addChild(dynamic_cast<Node*>(handle1));
		handle->addChild(dynamic_cast<Node*>(handle2));
		handle_line_trans->addChild(dynamic_cast<Node*>(handle_line));
		handle->addChild(dynamic_cast<Node*>(handle_line_trans));
		track->addChild(dynamic_cast<Node*>(handle));
	}
}

Track::~Track()
{
	delete base;
}

void Track::draw(glm::mat4 C, GLuint program)
{
	base->draw(C, program);
}

void Track::update_pos()
{
	double curr_time = glfwGetTime();
	float delta = 0.01f;
	if (float(curr_time - prev_time) >= delta)
	{
		prev_time = curr_time;
		if (!isEnergy)
		{
			velocity = 0.05f;
			if (!isPaused)
			{
				glm::vec3 temp = ball_point;
				get_next_point(all_curve_points, ball_point, closest_index, velocity, C);
				ball->update(glm::translate(ball_point - temp));
			}
		}
		else
		{
			velocity = 0.05f*glm::sqrt((max_height - ball_point.z));
			if (!isPaused)
			{
				glm::vec3 temp = ball_point;
				get_next_point(all_curve_points, ball_point, closest_index, velocity, C);
				ball->update(glm::translate(ball_point - temp));
			}
		}
	}

}

// If move_right, then the handle point is curr_point + 1. Else it's -1.
void Track::toggle_point(bool move_right)
{
	int prev_point = curr_point;

	if (curr_point == 1 || curr_point == 23)
	{
		handle_lines[0]->update_color(glm::vec3(1, 1, 0));
	}
	else
	{
		handle_lines[(curr_point + 1) / 3]->update_color(glm::vec3(1, 1, 0));
	}

	if (move_right)
	{
		curr_point++;
		// We don't want to move interpolating points.
		//if (curr_point % 3 == 0)
		//{
		//	curr_point++;
		//}
		// We don't want to go out of bounds of all_points vector.
		if (curr_point > 23)
		{
			curr_point = 1;
		}

		// Also need to update the current handle (same as current point, but in a different vector).
		if (curr_point % 3 != 0)
		{
			curr_handle++;
			if (curr_handle > 15)
			{
				curr_handle = 0;
			}
		}
	}
	else
	{
		curr_point--;
		//if (curr_point % 3 == 0)
		//{
		//	curr_point--;
		//}
		if (curr_point < 0)
		{
			curr_point = 23;
		}

		if (curr_point % 3 != 0)
		{
			curr_handle--;
			if (curr_handle < 0)
			{
				curr_handle = 15;
			}
		}
	}

	sel_point->update(glm::translate(all_points[curr_point] - all_points[prev_point]));

	if (curr_point == 1 || curr_point == 23)
	{
		handle_lines[0]->update_color(glm::vec3(0, 1, 0));
	}
	else
	{
		handle_lines[(curr_point + 1) / 3]->update_color(glm::vec3(0, 1, 0));
	}
}

// Move a control point by amount equal to displacement.
void Track::move_point(glm::vec3 displacement)
{
	all_points[curr_point] += displacement;
	sel_point->update(glm::translate(displacement));

	if (curr_point == 0)
	{
		all_points[23] += displacement;
		all_points[1] += displacement;
		interp_points[0]->update(glm::translate(displacement));
		handle_points[0]->update(glm::translate(displacement));
		handle_points[15]->update(glm::translate(displacement));
		handle_lines[0]->update_points(std::vector<glm::vec3>{ all_points[1], all_points[23] });
	}
	else if (curr_point % 3 == 0)
	{
		all_points[curr_point - 1] += displacement;
		all_points[curr_point + 1] += displacement;
		interp_points[curr_point / 3]->update(glm::translate(displacement));
		handle_points[(curr_point / 3) * 2 - 1]->update(glm::translate(displacement));
		handle_points[(curr_point / 3) * 2]->update(glm::translate(displacement));
		handle_lines[curr_point/3]->update_points(std::vector<glm::vec3>{ all_points[curr_point+1], all_points[curr_point-1] });
	}
	
	// Move corresponding handle point by equal and opposite amount.
	// Also need to move the transform corresponding to that handle point.
	else if (curr_point == 1)
	{
		handle_points[0]->update(glm::translate(displacement));
		all_points[23] -= displacement;
		handle_points[15]->update(glm::translate(-displacement));
		handle_lines[0]->update_points(std::vector<glm::vec3>{ all_points[curr_point], all_points[23] });
	}
	else if (curr_point == 23)
	{
		handle_points[15]->update(glm::translate(displacement));
		all_points[1] -= displacement;
		handle_points[0]->update(glm::translate(-displacement));
		handle_lines[0]->update_points(std::vector<glm::vec3>{ all_points[curr_point], all_points[1] });
	}
	else if (curr_point % 3 == 1)
	{
		//handle_points[curr_handle]->update(glm::translate(displacement));
		handle_points[(curr_point-1)/3-1]->update(glm::translate(displacement));
		all_points[curr_point - 2] -= displacement;
		//handle_points[curr_handle-1]->update(glm::translate(-displacement));
		handle_points[(curr_point-1)/3]->update(glm::translate(-displacement));
		handle_lines[(curr_point+1)/3]->update_points(std::vector<glm::vec3>{ all_points[curr_point], all_points[curr_point - 2] });
	}
	else if (curr_point % 3 == 2)
	{
		//handle_points[curr_handle]->update(glm::translate(displacement));
		handle_points[(curr_point + 1) / 3]->update(glm::translate(displacement));
		all_points[curr_point + 2] -= displacement;
		//handle_points[curr_handle + 1]->update(glm::translate(-displacement));
		handle_points[(curr_point + 1) / 3+1]->update(glm::translate(displacement));
		handle_lines[(curr_point + 1) / 3]->update_points(std::vector<glm::vec3>{ all_points[curr_point], all_points[curr_point + 2] });
	}

	// Recalculate curve. 
	// TODO: This can be optimized (whole curve doesn't have to be recalculatd).
	all_curve_points.clear();
    C.clear();
	for (int i = 0; i < 7; ++i)
	{
		calc_curve(std::vector<glm::vec3>{all_points[3 * i], all_points[3 * i + 1], all_points[3 * i + 2], all_points[3 * i + 3]}, all_curve_points, C);
	}
	std::vector<glm::vec3> last_seg = { all_points[21], all_points[22], all_points[23], all_points[0] };
	calc_curve(last_seg, all_curve_points, C);
	track_curve->update_points(all_curve_points);
	set_max_height(); // Reset max height
}

void Track::set_max_height()
{
	max_height = 0;
	for (auto iter = all_curve_points.begin(); iter != all_curve_points.end(); ++iter)
	{
		if ((*iter).z > max_height) max_height = (*iter).z;
	}
	max_height += (max_height / 100.0f);
}

void Track::calc_curve(std::vector<glm::vec3> control_points, std::vector<glm::vec3>& curve_points, std::vector<glm::mat4> &coeffs)
{
	glm::mat4 P = glm::mat4(1);
	P[0] = glm::vec4(control_points[0],0);
	P[1] = glm::vec4(control_points[1], 0);
	P[2] = glm::vec4(control_points[2], 0);
	P[3] = glm::vec4(control_points[3], 0);
	glm::mat4 B = glm::mat4(1);
	B[0] = glm::vec4{ -1.0f, 3.0f, -3.0f, 1.0f };
	B[1] = glm::vec4{ 3.0f, -6.0f, 3.0f, 0.0f };
	B[2] = glm::vec4{ -3.0f, 3.0f, 0.0f, 0.0f };
    B[3] = glm::vec4{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::mat4 C = P * B;
    coeffs.emplace_back(C);
	// In parametric equations, 0 <= t <= 1. We'll use 150 points between 0 and 1.
	float delta = 1.0f / 150.0f;
	float curr = 0.0f;
	for (int i = 0; i < 150; ++i)
	{
        glm::vec3 point = glm::vec3(C * glm::vec4{curr*curr*curr, curr*curr, curr, 1});
		curve_points.emplace_back(point);
		curr += delta;
	}
}

void Track::get_next_point(std::vector<glm::vec3> curve_points, glm::vec3 &point, int &index, float speed, std::vector<glm::mat4> coeffs)
{
    float length = 0;
    float prev_length = 0;
    float delta = 0.001;
    float dt = 1.0/300.0f;
    
    glm::vec3 temp_point = point;
    // First search for first point already on bezier curve that has a greater length than speed
    while(length - speed < 0)
    {
        prev_length = length;
        length += glm::length(temp_point - curve_points[index]);
        temp_point = curve_points[index];
        ++index;
        if(index >= curve_points.size()) index = 0;
    }
    int prev_index = index - 1;
    if(index < 0) prev_index = curve_points.size() - 1;
    point = curve_points[index];

	// The true point is between temp_point (smaller) and point.
	glm::vec3 smaller_point = curve_points[prev_index];

	float init_length = prev_length;
    
    // Find the "amount of speed" remaining after getting to prev_index.
    speed = speed-prev_length;
	//std::cout << "Previous length: " << prev_length << std::endl;
	//std::cout << "Speed remaining: " << speed << std::endl;

    // Find the "amount of length" left. Should be geater than amount of speed left.
    length = length - prev_length;
	//std::cout << "length remainig: " << length << std::endl;

    // Get the correct C matrix to use (to interpolate Bezier fxn).
    // Also get the correct t value corresponding to the current point in the current Bezier curve (e.g. the 6th of 8 Bezier curves).
    //std::cout << "Index is: " << prev_index << std::endl;
    //std::cout << "Coeff index is: " << (int) prev_index / 150 << std::endl;
    //std::cout << "Size of coeffs is: " << coeffs.size();
    glm::mat4 coeff = coeffs[(int) prev_index/150];
	float t = prev_index % 150 * 1.0f / 150.0f;
    
    // The desired point is between prev_index and index. Do a binary search for the point on the curve
    // that hasn't already been calculated that is close to speed within some delta.
	std::cout << "Error: " << length - speed << std::endl;
	std::cout << "t is: " << t << std::endl;
	while ((length - speed < -delta || length - speed > delta) && dt > 0.000001)
    {
		/*std::cout << "Smaller point" << std::endl;
		print_point(smaller_point);
		std::cout << "Temp point" << std::endl;
		print_point(temp_point);*/
        length = glm::length(smaller_point - temp_point);
        // Need to move farther
        if(length - speed < 0)
        {
			//std::cout << "GO FARTHER" << std::endl;
            t = t + dt;
			//std::cout << "t is now: " << t << std::endl;
            temp_point = glm::vec3(coeff * glm::vec4{t*t*t, t*t, t, 1});
			//print_point(temp_point);
        }
        else if(length - speed > 0)
        {
			//std::cout << "GO CLOSER" << std::endl;
            t = t - dt;
			//std::cout << "t is now: " << t << std::endl;
            temp_point = glm::vec3(coeff * glm::vec4{t*t*t, t*t, t, 1});
			//print_point(temp_point);
        }
        dt /= 2.0f;
		std::cout << "Error: " << length - speed << std::endl;
		std::cout << "t: " << t << std::endl;
    }
	//std::cout << "final_point" << std::endl;
	//print_point(temp_point);
	std::cout << "Velocity: " << init_length + length << std::endl;
	point = temp_point;
	std::cout << "-------------------------------------------------------" << std::endl;
}


void Track::print_point(glm::vec3 &point)
{
    std::cout << "Point: (";
    for(int i = 0; i < 3; ++i)
    {
        std::cout << point[i] << ",";
    }
    std::cout << ")" << std::endl;
}

void Track::print_mat(glm::mat4& mat)
{
	std::cout << "Matrix:" << std::endl;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			std::cout << mat[j][i] << "  ";
		}
		std::cout << std::endl;
	}
}

void Track::pause()
{
	isPaused = !isPaused;
}

void Track::energy_on()
{
	isEnergy = !isEnergy;
}

glm::vec3 Track::get_tangent()
{
	return all_curve_points[closest_index] - ball_point;
}
glm::vec3 Track::get_ball_point()
{
	return ball_point;
}

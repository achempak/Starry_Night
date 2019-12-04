#ifndef TERRAIN_H
#define TERRAIN_H

#include "Node.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <string>
#include <vector>
#include <iomanip>
#include <random>
#include <math.h>

class Terrain : public Node
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals_vertex;

	glm::mat4 model;
	GLuint vao;
	GLuint vbos[2];

	// Color properties. Will change these later.
	glm::vec3 ambient;

	// Flags
	bool wireframe_flag;

	void diamond(std::vector<std::vector<float>>& height, int x, int z, int r, float eps, float range, std::mt19937 gen);
	void square(std::vector<std::vector<float>>& height, std::vector<std::vector<bool>>& flags, int x, int z, int r, float eps, float range, std::mt19937 gen);
	void diamond_square(std::vector<std::vector<float>>& height, std::vector<std::vector<bool>>& flags, int x, int z, int r, float eps, float range, std::mt19937 gen);

public:
	// Pretty print 2D vector
	template <typename T>
	static void print2d(std::vector<std::vector<T>> v)
	{
		for (int i = 0; i < v.size(); ++i)
		{
			for (int j = 0; j < v[i].size(); ++j)
			{
				std::cout << std::setprecision(2) << v[i][j] << "  ";
			}
			std::cout << std::endl;
		}
	};

	Terrain(int size, std::vector<float> corners);
	~Terrain();
	void wireframe(bool flag);
	bool getWireframe();
	virtual void draw(glm::mat4 C, GLuint program);
	virtual void update(glm::mat4 C) {};
	virtual void updatePos(glm::mat4 C) {};
	virtual std::string getName() {
		return "Terrain";
	};
};

#endif // !TERRAIN_H


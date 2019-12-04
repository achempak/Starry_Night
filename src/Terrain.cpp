#include "Terrain.h"

void Terrain::diamond(std::vector<std::vector<float>>& height, int x, int z, int r, float eps, std::mt19937 gen)
{
	// Subarray corners are: height[x-r/2][z-r/2], height[x+r/2][z-r/2], height[x-r/2][z+r/2], height[x+r/2][z+r/2]
	// Center is height[x][z]
	// r is diameter of diamond
	float range = 100.0f;
	std::uniform_real_distribution<float> dis(-range, range);
	height[x][z] = (height[x - r / 2][z - r / 2] + height[x + r / 2][z - r / 2] + height[x - r / 2][z + r / 2] + height[x + r / 2][z + r / 2]) / 4 + dis(gen) * eps;
}

void Terrain::square(std::vector<std::vector<float>>& height, std::vector<std::vector<bool>>& flags, int x, int z, int r, float eps, std::mt19937 gen)
{
	// Center is height[x][z]
	// Flags contains a flag for each point to see if it has already been calculated
	float range = 100.0f;

	// Left point
	if (!flags[x - r / 2][z])
	{
		std::uniform_real_distribution<float> dis(-range, range);
		if (x - r > 0)
		{
			height[x - r / 2][z] = (height[x][z] + height[x - r][z] + height[x - r / 2][z - r / 2] + height[x - r / 2][z + r / 2]) / 4 + dis(gen) * eps;
		}
		else
		{
			height[x - r / 2][z] = (height[x][z] + height[x - r / 2][z - r / 2] + height[x - r / 2][z + r / 2]) / 3 + dis(gen) * eps;
		}
		flags[x - r / 2][z] = true;
	}

	// Right point
	if (!flags[x + r / 2][z])
	{
		std::uniform_real_distribution<float> dis(-range, range);
		if (x + r < height[0].size())
		{
			height[x + r / 2][z] = (height[x][z] + height[x + r][z] + height[x + r / 2][z - r / 2] + height[x - r / 2][z + r / 2]) / 4 + dis(gen) * eps;
		}
		else
		{
			height[x + r / 2][z] = (height[x][z] + height[x + r / 2][z - r / 2] + height[x + r / 2][z + r / 2]) / 3 + dis(gen) * eps;
		}
		flags[x + r / 2][z] = true;
	}

	// Top point
	if (!flags[x][z - r / 2])
	{
		std::uniform_real_distribution<float> dis(-range, range);
		if (z - r > 0)
		{
			height[x][z - r / 2] = (height[x][z] + height[x][z - r] + height[x - r / 2][z - r / 2] + height[x + r / 2][z - r / 2]) / 4 + dis(gen) * eps;
		}
		else
		{
			height[x][z - r / 2] = (height[x][z] + height[x - r / 2][z - r / 2] + height[x + r / 2][z - r / 2]) / 3 + dis(gen) * eps;
		}
		flags[x][z - r / 2] = true;
	}

	// Bottom point
	if (!flags[x][z + r / 2])
	{
		std::uniform_real_distribution<float> dis(-range, range);
		if (z + r < height[0].size())
		{
			height[x][z + r / 2] = (height[x][z] + height[x][z + r] + height[x - r / 2][z + r / 2] + height[x + r / 2][z + r / 2]) / 4 + dis(gen) * eps;
		}
		else
		{
			height[x][z + r / 2] = (height[x][z] + height[x - r / 2][z + r / 2] + height[x + r / 2][z + r / 2]) / 3 + dis(gen) * eps;
		}
		flags[x][z + r / 2] = true;
	}
}

void Terrain::diamond_square(std::vector<std::vector<float>>& height, std::vector<std::vector<bool>>& flags, int x, int z, int r, float eps, std::mt19937 gen)
{
	if (r < 1) return;

	float shrink_factor = 0.6f;
	diamond(height, x, z, r, eps, gen);
	square(height, flags, x, z, r, eps, gen);

	// Each square spawns four more diamonds
	diamond_square(height, flags, x - r / 4, z - r / 4, r / 2, eps * shrink_factor, gen);
	diamond_square(height, flags, x + r / 4, z - r / 4, r / 2, eps * shrink_factor, gen);
	diamond_square(height, flags, x - r / 4, z + r / 4, r / 2, eps * shrink_factor, gen);
	diamond_square(height, flags, x + r / 4, z + r / 4, r / 2, eps * shrink_factor, gen);
}

Terrain::Terrain(int size, std::vector<float> corners) : ambient(glm::vec3(1.0f, 0.1f, 0.1f)), wireframe_flag(false)
{
	int n = (int)std::pow(2, (double)size) + 1;
	std::vector<std::vector<float>> height_i(n, std::vector<float>(n));
	height_i[0][0] = corners[0];
	height_i[0][n - 1] = corners[1];
	height_i[n - 1][0] = corners[2];
	height_i[n - 1][n - 1] = corners[3];

	std::vector<std::vector<bool>> flags(n, std::vector<bool>(n));

	std::random_device rd;
	std::mt19937 gen(rd());
	diamond_square(height_i, flags, n / 2, n / 2, n - 1, 1.0f, gen);

	/**********************************************************************************/
	// Let us smooth the heightmap using a Gaussian filter with sigma=5.

	std::vector<float> kernel = { 0.192077f,0.203914f, 0.208019f, 0.203914f, 0.192077f };
	std::vector<std::vector<float>> height_first_pass(n, std::vector<float>(n));
	std::vector<std::vector<float>> height(n, std::vector<float>(n));
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			float sum = 0.0f;
			for (int k = -2; k <= 2; ++k)
			{
				int index = j + k;
				if (index < 0)
				{
					sum += height_i[i][0] * kernel[k+2];
				}
				else if (index >= n)
				{
					sum += height_i[i][n - 1] * kernel[k + 2];
				}
				else
				{
					sum += height_i[i][index] * kernel[k + 2];
				}
			}
			height_first_pass[i][j] = sum;
		}
	}
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			float sum = 0.0f;
			for (int k = -2; k <= 2; ++k)
			{
				int index = j + k;
				if (index < 0)
				{
					sum += height_first_pass[0][i] * kernel[k + 2];
				}
				else if (index >= n)
				{
					sum += height_first_pass[n - 1][i] * kernel[k + 2];
				}
				else
				{
					sum += height_first_pass[index][i] * kernel[k + 2];
				}
				height[i][j] = sum;
			}
		}
	}


	/*********************************************************************************/
	// Make actual vec3 points. Number of triangles per row is (n-1) * 2.
	// Number of rows is n-1. We have 2*(n-1)^2 triangles total.
	// The size of the grid will by (grid_size x grid_size).

	std::cout << "MAKING VERTICES." << std::endl;
	float grid_size = 500;
	for (int i = 0; i < n - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			//Together, the two triangles make up a square.
			// Left triangle (counter-clockwise)
			vertices.emplace_back(glm::vec3(((grid_size) / (float)(n - 1) * j), (height[i][j]), ((grid_size) / (float)(n - 1) * i))); // Top left
			vertices.emplace_back(glm::vec3(((grid_size) / (float)(n - 1) * (j + 1)), (height[i][j + 1]), ((grid_size) / (float)(n - 1) * i))); // Top right
			vertices.emplace_back(glm::vec3(((grid_size) / (float)(n - 1) * j), (height[i + 1][j]), ((grid_size) / (float)(n - 1) * (i + 1)))); // Bottom left

			// Right triangle (counter-clockwise)
			vertices.emplace_back(glm::vec3(((grid_size) / (float)(n - 1) * (j + 1)), (height[i][j + 1]), ((grid_size) / (float)(n - 1) * i))); // Top right
			vertices.emplace_back(glm::vec3(((grid_size) / (float)(n - 1) * (j + 1)), (height[i + 1][j + 1]), ((grid_size) / (float)(n - 1) * (i + 1)))); // Bottom right
			vertices.emplace_back(glm::vec3(((grid_size) / (float)(n - 1) * j), (height[i + 1][j]), ((grid_size) / (float)(n - 1) * (i + 1)))); // Bottom left

		}
	}
	std::cout << "Size of vertices: " << vertices.size() << std::endl;

	/*********************************************************************************/
	// Make normals PER FACE.

	std::cout << "MAKING NORMALS PER FACE." << std::endl;
	std::vector<glm::vec3> normals_face;
	for (int i = 0; i < vertices.size(); i = i + 6)
	{
		glm::vec3 cross = glm::cross(vertices[i] - (vertices[i + 2]), vertices[i] - (vertices[i + 1]));
		normals_face.emplace_back(cross);
		cross = glm::cross((vertices[i + 4]) - (vertices[i + 3]), (vertices[i + 4]) - (vertices[i + 5]));
		normals_face.emplace_back(cross);
		// for(int j = 0; j < 3; ++j)
		// {
		// 	glm::vec3 cross = glm::cross(vertices[i*3] - (vertices[i*3+2]), vertices[i*3] - (vertices[i*3+1]));
		// 	normals_face.emplace_back(cross);
		// 	cross = glm::cross((vertices[i*3+4]) - (vertices[i*3+3]), (vertices[i*3+4]) - (vertices[i*3+5]));
		// 	normals_face.emplace_back(cross);
		// }
	}
	std::cout << "Size of normals_face: " << normals_face.size() << std::endl;

	/*********************************************************************************/
	// Make normals PER VERTEX.
	// Number of vetices per row is (n-1) * 6.

	std::cout << "MAKING NORMALS PER VERTEX." << std::endl;
	int curr_face = -2;
	for (int i = 0; i < vertices.size(); ++i)
	{
		glm::vec3 sum = glm::vec3(0.0f);
		// Every six vertices (so every two triangles), incremenet the normals_face index by 2.
		if (i % 6 == 0) curr_face += 2;

		if (i < (n - 1) * 6)
		{
			// Index of top left corner in vertices vector
			if (i == 0)
			{
				sum += normals_face[curr_face];
			}
			// Top right corner
			else if (i == (n - 1) * 6 - 3 || i == (n - 1) * 6 - 5 == 0)
			{
				sum += normals_face[curr_face] + normals_face[curr_face + 1];
				sum /= 2.0f;
			}
			// Vertex on top edge
			else if (i % 6 == 0 || (i - 1) % 6 == 0 || (i - 3) % 6 == 0)
			{
				// Differentiate between the two types of vertices on top edge
				if (i % 6 == 0)
				{
					sum += normals_face[curr_face - 2] + normals_face[curr_face - 1] + normals_face[curr_face];
				}
				else
				{
					sum += normals_face[curr_face] + normals_face[curr_face + 1] + normals_face[curr_face + 2];
				}
				sum /= 3.0f;
			}
			normals_vertex.emplace_back(sum);
		}
		else if (i > (n - 1) * 6 * (n - 2))
		{
			// Bottom left corner
			if (i == (n - 1) * 6 * (n - 2) + 2 || i == (n - 1) * 6 * (n - 2) + 4)
			{
				sum += normals_face[curr_face] + normals_face[curr_face + 1];
				sum /= 2.0f;
			}
			// Bottom right corner
			else if (i == vertices.size() - 2)
			{
				sum += normals_face[curr_face + 1];
			}
			// Bottom edge
			else if ((i - 2) % 6 == 0 || (i - 4) % 6 == 0 || (i - 5) % 6 == 0)
			{
				// Differentiate between the two types of vertices on bottom edge
				if ((i - 2) % 6 == 0 || (i - 5) % 6 == 0)
				{
					sum += normals_face[curr_face - 2] + normals_face[curr_face - 1] + normals_face[curr_face];
				}
				else
				{
					sum += normals_face[curr_face] + normals_face[curr_face + 1] + normals_face[curr_face + 2];
				}
				sum /= 3.0f;
			}
			normals_vertex.emplace_back(sum);
		}
		// Vertex on the left edge
		else if (i % ((n - 1) * 6) == 0 || (i - 2) % ((n - 1) * 6) == 0 || (i - 5) % ((n - 1) * 6) == 0)
		{
			// Differentiate between the two types of vertices on left edge
			if (i % ((n - 1) * 6) == 0)
			{
				sum += normals_face[curr_face] + normals_face[curr_face - (n - 1) * 2] + normals_face[curr_face - (n - 1) * 2 + 1];
			}
			else
			{
				sum += normals_face[curr_face] + normals_face[curr_face + 1] + normals_face[curr_face + (n - 1) * 2];
			}
			sum /= 3.0f;
			normals_vertex.emplace_back(sum);
		}
		// Vertex on right edge
		else if ((i + 2) % ((n - 1) * 6) == 0 || (i + 3) % ((n - 1) * 6) == 0 || (i + 5) % ((n - 1) * 6) == 0)
		{
			// Differentiate between the two types of vertices on right edge
			if ((i + 5) % ((n - 1) * 6) == 0)
			{
				sum += normals_face[curr_face - ((n - 1) * 2 - 1)] + normals_face[curr_face] + normals_face[curr_face + 1];
			}
			else
			{
				sum += normals_face[curr_face + 1] + normals_face[curr_face + (n - 1) * 2] + normals_face[curr_face + (n - 1) * 2 + 1];
			}
			sum /= 3.0f;
			normals_vertex.emplace_back(sum);
		}
		// Any interior vertex
		else
		{
			if ((i - 4) % 6 == 0)
			{
				sum += normals_face[curr_face + 1] + normals_face[curr_face + 2] + normals_face[curr_face + 3] + normals_face[curr_face + (n - 1) * 2 - 1] + normals_face[curr_face + (n - 1) * 2 + 1] + normals_face[curr_face + (n - 1) * 2 + 2];
			}
			else if ((i - 2) % 6 == 0 || (i - 5) % 6 == 0)
			{
				sum += normals_face[curr_face - 1] + normals_face[curr_face] + normals_face[curr_face + 1] + normals_face[curr_face + (n - 1) * 2 - 2] + normals_face[curr_face + (n - 1) * 2 - 1] + normals_face[curr_face + (n - 1) * 2];
			}
			else if ((i - 1) % 6 == 0 || (i - 3) % 6 == 0)
			{
				sum += normals_face[curr_face - (n - 1) * 2 + 1] + normals_face[curr_face - (n - 1) * 2 + 2] + normals_face[curr_face - (n - 1) * 2 + 3] + normals_face[curr_face] + normals_face[curr_face + 1] + normals_face[curr_face + 2];
			}
			else
			{
				// i % 6 == 0
				sum += normals_face[curr_face - (n - 1) * 2 - 1] + normals_face[curr_face - (n - 1) * 2] + normals_face[curr_face - (n - 1) * 2 + 1] + normals_face[curr_face - 2] + normals_face[curr_face - 1] + normals_face[curr_face];
			}
			sum /= 6.0f;
			normals_vertex.emplace_back(sum);
		}
	}
	std::cout << "DONE." << std::endl;
	std::cout << "Size of normals_vertex: " << normals_vertex.size() << std::endl;


	/*********************************************************************************/
	// Normalize the terrain.
	/*float window_width = 640;
	float window_height = 480;
	glm::vec3 max{ 0,0,0 };
	glm::vec3 min{ 0,0,0 };
	for (auto iter = vertices.begin(); iter != vertices.end(); ++iter)
	{
		glm::vec3 point = *iter;
		if (point.x > max[0]) max[0] = point.x;
		else if (point.x < min[0]) min[0] = point.x;
		if (point.y > max[1]) max[1] = point.y;
		else if (point.y < min[1]) min[1] = point.y;
		if (point.z > max[2]) max[2] = point.z;
		else if (point.z < min[2]) min[2] = point.z;
	}
	glm::vec3 center{ (max[0] + min[0]) / 2, (max[1] + min[1]) / 2, (max[2] + min[2]) / 2 };
	std::cout << "Center of terrain is " << (max[0] + min[0]) / 2 << ", " << (max[1] + min[1]) / 2 << ", " << (max[2] + min[2]) / 2 << std::endl;
	std::vector<float> dist(vertices.size());
	float max_dist = 0;
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i] = vertices[i] - center;
		dist[i] = glm::dot(vertices[i], vertices[i]);
		if (dist[i] > max_dist) max_dist = dist[i];
	}
	max_dist = sqrt(max_dist);
	float multiplier = 1;
	float magic_scale_factor = 0.02;
	if (window_width < window_height)
	{
		multiplier = window_width / max_dist;
	}
	else multiplier = window_height / max_dist;
	std::cout << "Multiplier for " << "Terrain" << " is " << multiplier << std::endl;
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i] = magic_scale_factor * multiplier * vertices[i];
	}*/

	/*********************************************************************************/
	// Deal with GPU stuff.
	model = glm::mat4(1);
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbos);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals_vertex.size(), normals_vertex.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	std::cout << "Done initializing Terrain." << std::endl;
}

Terrain::~Terrain()
{
	glDeleteBuffers(2, vbos);
	glDeleteVertexArrays(1, &vao);
}

void Terrain::draw(glm::mat4 C, GLuint program)
{
	glUseProgram(program);
	GLuint modelLoc = glGetUniformLocation(program, "model");
	GLuint color = glGetUniformLocation(program, "color");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(C * model));
	glUniform3fv(color, 1, glm::value_ptr(ambient));

	glBindVertexArray(vao);
	if (!wireframe_flag)
	{
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}
	else
	{
		glDrawArrays(GL_LINES, 0, vertices.size());
	}
	glBindVertexArray(0);
}

void Terrain::wireframe(bool flag)
{
	if (flag) wireframe_flag = true;
	else wireframe_flag = false;
}

bool Terrain::getWireframe()
{
	return wireframe_flag;
}
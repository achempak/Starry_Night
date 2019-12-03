#include "RGeometry.h"
#include <iostream>
#include <fstream>
#include <sstream>

RGeometry::RGeometry(std::string objFilename, std::string geo_name, GLuint skyTexture, GLuint program) : name(geo_name), skyboxTexture(skyTexture), prog(program)
{
	float width = 640;
	float height = 480;
	glm::vec3 max{ 0,0,0 };
	glm::vec3 min{ 0,0,0 };
	std::vector<int> face_indices;
	std::vector<glm::vec3> input_vertices;
	std::vector<glm::vec3> input_normals;
	std::ifstream objFile(objFilename);
	if (objFile.is_open())
	{
		std::string line;
		std::cout << "Reading from " << objFilename << std::endl;
		while (std::getline(objFile, line))
		{
			std::stringstream ss;
			ss << line;
			std::string label;
			ss >> label;
			if (label == "v")
			{
				glm::vec3 point;
				ss >> point.x >> point.y >> point.z;
				input_vertices.push_back(point);
				if (point.x > max[0]) max[0] = point.x;
				else if (point.x < min[0]) min[0] = point.x;
				if (point.y > max[1]) max[1] = point.y;
				else if (point.y < min[1]) min[1] = point.y;
				if (point.z > max[2]) max[2] = point.z;
				else if (point.z < min[2]) min[2] = point.z;
			}
			else if (label == "vn")
			{
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;
				input_normals.push_back(normal);
			}
			else if (label == "f")
			{
				// Format v/vt/vn
				// Store face data in vector face_indices such that it gets entered v,vt,vn,v,...
				std::string in;
				std::string temp = "";

				for (int i = 0; i < 3; ++i)
				{
					ss >> in;
					//std::cout << in << std::endl;
					for (char& c : in)
					{
						if (c != '/')
						{
							temp += c;
						}
						else
						{
							int index;
							// If statement accounts for cases when data is in the form 
							// v//vn instead of the standard v/vf/vn. If face data is given
							// as just a list of vertices, then we're a bit screwed.
							if (temp != "")
							{
								index = std::stoi(temp) - 1;
							}
							else
							{
								index = 0;
							}
							face_indices.push_back(index);
							temp = "";
						}
					}
					face_indices.push_back(std::stoi(temp) - 1);
					temp = "";
				}
			}
		}
	}
	else
	{
		std::cerr << "Can't open file " << objFilename << std::endl;
	}
	objFile.close();

	if (geo_name == "bounding")
	{
		float radius = glm::length(input_vertices[0]);
		std::cout << "RADIUS IS: " << radius << std::endl;
	}

	// Align vertices and normals
	int index = 0;
	for (int i = 0; i < face_indices.size(); i += 3)
	{
		points.push_back(input_vertices[face_indices[i]]);
		normals.push_back(input_normals[face_indices[i + 2]]);
		indices.push_back(index);
		++index;
	}

	// Repositioning and determining scale factor
	glm::vec3 center{ (max[0] + min[0]) / 2, (max[1] + min[1]) / 2, (max[2] + min[2]) / 2 };
	std::cout << "Center is " << (max[0] + min[0]) / 2 << ", " << (max[1] + min[1]) / 2 << ", " << (max[2] + min[2]) / 2 << std::endl;
	std::vector<float> dist(points.size());
	max_dist = 0;
	for (int i = 0; i < points.size(); ++i)
	{
		points[i] = points[i] - center;
		dist[i] = glm::dot(points[i], points[i]);
		if (dist[i] > max_dist) max_dist = dist[i];
	}
	max_dist = sqrt(max_dist);

	float multiplier = 1;
	float magic_scale_factor = 0.02;
	if (width < height)
	{
		multiplier = width / max_dist;
	}
	else multiplier = height / max_dist;
	std::cout << "Multiplier for " << objFilename << " is " << multiplier << std::endl;
	for (int i = 0; i < points.size(); ++i)
	{
		points[i] = magic_scale_factor * multiplier * points[i];
	}


	// Set the model matrix to an identity matrix.
	model = glm::mat4(1);
	pos = model;
	// Set the color.
	//color = glm::vec3(0.5f, 1.0f, 0.2f);
	
	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(3, vbos);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0.
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO. We will use it to store the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
	// Pass in the data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(),
		indices.data(), GL_STATIC_DRAW);

	// Bind to the third VBO. Used to store normals.
	glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 1.
	// We will be able to access normals through it.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	std::cout << "RGeometry VAO: " << vao << std::endl;
	std::cout << "Done initializing object." << std::endl;
}

RGeometry::RGeometry(std::string objFilename, GLuint skyTexture, GLuint program) : RGeometry::RGeometry(objFilename, "", skyTexture, prog) {}


RGeometry::~RGeometry()
{
	// Delete the VBO and the VAO.
	glDeleteBuffers(3, vbos);
	glDeleteVertexArrays(1, &vao);
}

void RGeometry::draw(glm::mat4 C, GLuint program)
{
	glm::mat4 temp = C * model;
	// Alternatively, if all models use the same shader, can just pass in individual locations
	glUseProgram(prog);
	//std::cout << "Reflection using program " << program << std::endl;

	// Get locations of parameters in shader
	GLuint modelLoc = glGetUniformLocation(prog, "model");

	// Assign values to different parameters
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(temp));

	// Bind to the VAO.
	glBindVertexArray(vao);
	// Bind skybox texture.
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	// Draw triangles using the indices in the second VBO, which is an
	// elemnt array buffer.
	glDrawElements(GL_TRIANGLES, sizeof(int) * indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void RGeometry::updatePos(glm::mat4 C)
{
	pos = C * model;
}

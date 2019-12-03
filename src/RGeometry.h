#ifndef RGeometry_hpp
#define RGeometry_hpp

#include "Node.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class RGeometry : public Node
{
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<int> indices;

	glm::mat4 model;
	glm::mat4 pos;
	GLuint vao;
	GLuint vbos[3];
	float max_dist;

	std::string name;
	GLuint skyboxTexture;
	GLuint prog;

public:
	RGeometry(std::string filename, std::string name, GLuint skyTexture, GLuint program);
	RGeometry(std::string filename, GLuint skyTexture, GLuint program);
	~RGeometry();
	virtual void draw(glm::mat4 C, GLuint program);
	virtual void update(glm::mat4 C) {};
	virtual void updatePos(glm::mat4 C);
	virtual std::string getName() {
		return name;
	};
	glm::mat4 getPos() { return pos; };
};

#endif /* RGeometry_hpp */

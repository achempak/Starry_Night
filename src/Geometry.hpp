//
//  Geometry.hpp
//  Project1
//
//  Created by Aditya Chempakasseril on 10/28/19.
//  Copyright © 2019 Aditya Chempakasseril. All rights reserved.
//

#ifndef Geometry_hpp
#define Geometry_hpp

#include "Node.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class Geometry : public Node
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
    
    // Material properties
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    
    std::string name;
    bool bounding_on;

public:
    Geometry(std::string filename, std::string name);
    Geometry(std::string filename);
	Geometry(std::vector<glm::vec3> pts, std::vector<glm::vec3> norms, std::vector<int> inds, std::string name);
    ~Geometry();
    virtual void draw(glm::mat4 C, GLuint program);
    virtual void update(glm::mat4 C){};
    virtual void updatePos(glm::mat4 C);
	virtual std::string getName() {
		return name;
	};
    void setMaterial(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shine);
	void setMaterial(glm::vec3 amb);
    void toggle_bounding(bool on);
    glm::mat4 getPos(){ return pos; };
};

#endif /* Geometry_hpp */

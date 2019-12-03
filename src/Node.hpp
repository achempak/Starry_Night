//
//  Node.hpp
//  Project1
//
//  Created by Aditya Chempakasseril on 10/28/19.
//  Copyright © 2019 Aditya Chempakasseril. All rights reserved.
//

#ifndef Node_hpp
#define Node_hpp

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

class Node
{
public:
    virtual void draw(glm::mat4 C, GLuint program) = 0;
    virtual void update(glm::mat4 C) = 0;
	virtual std::string getName() = 0;
    virtual void updatePos(glm::mat4 C) = 0;
    
};

#endif /* Node_hpp */

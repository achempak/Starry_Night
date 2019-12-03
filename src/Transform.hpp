//
//  Transform.hpp
//  Project1
//
//  Created by Aditya Chempakasseril on 10/28/19.
//  Copyright © 2019 Aditya Chempakasseril. All rights reserved.
//

#ifndef Transform_hpp
#define Transform_hpp

#include "Node.hpp"
#include <list>

class Transform : public Node
{
private:
    glm::mat4 M;
    std::list<Node*>* children;
    std::string name;
    
public:
    Transform(glm::mat4 M, std::string name);
    Transform(glm::mat4 M);
	~Transform();
    void addChild(Node*);
    std::list<Node*>* getChildren();
    virtual void draw(glm::mat4 C, GLuint program);
    virtual void update(glm::mat4 C);
    virtual std::string getName();
    virtual void updatePos(glm::mat4 C);
};

#endif /* Transform_hpp */

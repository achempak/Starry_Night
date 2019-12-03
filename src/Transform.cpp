//
//  Transform.cpp
//  Project1
//
//  Created by Aditya Chempakasseril on 10/28/19.
//  Copyright © 2019 Aditya Chempakasseril. All rights reserved.
//

#include "Transform.hpp"

//std::list<Node*> children;

Transform::Transform(glm::mat4 model, std::string n)
{
    M = model;
    children = new std::list<Node*>;
    name = n;
}

Transform::Transform(glm::mat4 model)
{
    M = model;
    children = new std::list<Node*>;
    name = "";
}

Transform::~Transform()
{
	delete children;
}

void Transform::addChild(Node* child)
{
    children->push_back(child);
}

std::list<Node*>* Transform::getChildren()
{
    return children;
}

void Transform::draw(glm::mat4 C, GLuint program)
{
    // Need to update the model
    //M = C * M;
    for(auto iter = children->begin(); iter != children->end(); ++iter)
    {
        (*iter)->draw(C * M, program);
    }
}

void Transform::update(glm::mat4 C)
{
    M = C * M;
}

void Transform::updatePos(glm::mat4 C)
{
    for(auto iter = children->begin(); iter != children->end(); ++iter)
    {
        (*iter)->updatePos(C * M);
    }
}

std::string Transform::getName()
{
    return name;
}

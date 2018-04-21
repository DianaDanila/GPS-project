//
//  Shader.hpp
//  Lab3
//
//  Created by CGIS on 05/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Shader_hpp
#define Shader_hpp

#include <stdio.h>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

namespace gps {

class Shader
{
public:
    GLuint shaderProgram;
    void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
    void useShaderProgram();

	void sendMat3(glm::mat3 matrice, std::string name);
	void sendMat4(glm::mat4 matrice, std::string name);
	void sendVec3(glm::vec3 vector, std::string name);
	void sendVec4(glm::vec4 vector, std::string name);
	void sendFloat(float myFloat, std::string name);
	void sendInt(float myInt, std::string name);

private:
    std::string readShaderFile(std::string fileName);
    void shaderCompileLog(GLuint shaderId);
    void shaderLinkLog(GLuint shaderProgramId);
};

}

#endif /* Shader_hpp */

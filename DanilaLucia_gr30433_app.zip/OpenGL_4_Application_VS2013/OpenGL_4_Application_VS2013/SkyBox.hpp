#pragma once

#ifndef Skybox_hpp
#define Skybox_hpp

#include <vector>
#include "GLEW\glew.h"
#include "stb_image.h"
#include <iostream>
#include "Shader.hpp"

class SkyBox {
	
	public:
		SkyBox();
		GLuint loadSkyCube(std::vector<const GLchar*> faces);
		void initSkyBox();
		GLuint getTextureID();
		void Draw(gps::Shader shader, glm::mat4 view, glm::mat4 projection);
		void Load(std::vector<const GLchar*> faces);

	private:

		GLuint textureID;
		GLuint VAO;
		GLuint VBO;
};

#endif
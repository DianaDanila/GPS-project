//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include <Windows.h>
#include <MMSystem.h>
#include <thread>
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"


glm::mat4 computeLightSpaceMatrix();

struct directionalLight{
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 color;
};

struct pointLight{
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 color;
	float constant;
	float linear;
	float quadratic;
};

struct fog{
	glm::vec3 position;
	glm::vec4 color;
	float density;
	//float factor;
};

int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

double oldx = (float)glWindowWidth / 2.0, oldy = (float)glWindowHeight/2.0;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;
glm::vec3 position = glm::vec3(100.0f, 50.0f, 100.0f);//position of the ship
glm::mat4 chasemodel = glm::mat4(1.0f);
glm::mat4 chasemodelA = glm::mat4(1.0f);
glm::mat4 chasemodelA2 = glm::mat4(1.0f);

directionalLight myLight;
pointLight myPointLight;
fog myFog;
fog myFogDark;
GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint lightAmbientLoc;
GLuint lightDiffuseLoc;
GLuint lightSpecularLoc;

gps::Camera myCamera(glm::vec3(9.5f, 0.0f, 9.5f), glm::vec3(0.0f, 0.0f, 0.0f));
float cameraSpeed = 5.f;
float difficulty = 0.5f;

bool pressedKeys[1024];
float angle = 0.0f;
float rotAngleTot = 0.0f;
float floorx = 10.0f, floory = 1.0f, floorz = 10.0f;
GLfloat deltaTime = 0.0f, currentFrameTime, lastFrameTime = glfwGetTime();
int chase = 0;

gps::Model3D myModel;
gps::Model3D myGround;
gps::Model3D myLeftSide;
gps::Model3D myBackSide;
gps::Model3D myRightSide;
gps::Model3D myDownSide;
gps::Model3D myAlien;
gps::Model3D myAlien2;
gps::Model3D myMonster;
gps::Model3D myPlanet;
gps::Model3D myPlanet2;
gps::Model3D myShip;
gps::Model3D myRock;
gps::Shader myCustomShader;
gps::Shader myShadowShader;
gps::Shader mySkyBoxShader;
gps::Shader myPlanetShader;
gps::Shader myRockShader;
SkyBox mySkyBox;

unsigned int depthMapFBO;
const unsigned int SHADOW_WIDTH = 10124, SHADOW_HEIGHT = 1024;
unsigned int depthMap;
GLuint textureID;

int rad = 4;
int i, j;
int appear = 0;//alien2
int t = 0;//appear monster

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();
	//set projection matrix
	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
	//send matrix data to shader
	myCustomShader.sendMat4(projection, "projection");

	mySkyBoxShader.useShaderProgram();
	mySkyBoxShader.sendMat4(projection, "projection");

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);

	glWindowWidth = width;
	glWindowHeight = height;
}

GLuint wireframe = 0;

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	double deltax = xpos - oldx;
	double deltay = oldy - ypos;//y e inversat
	oldy = ypos;
	oldx = xpos;
	float anglex = 0.02 * deltax;
	float angley = 0.02 * deltay;
	myCamera.rotate(angley, anglex);
}


void processMovement(float time, glm::vec3 &position)
{

	if (pressedKeys[GLFW_KEY_SPACE]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed * time, floorz);
	}

	if (pressedKeys[GLFW_KEY_X]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed * time, floorz);
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed * time, floorz);
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed * time, floorz);
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed * time, floorx);
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed * time, floorx);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::ROTATE_RIGHT, cameraSpeed * time, floorx);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::ROTATE_LEFT, cameraSpeed * time, floorx);
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::ROTATE_UP, cameraSpeed * time, floorx);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::ROTATE_DOWN, cameraSpeed * time, floorx);
	}

	if (pressedKeys[GLFW_KEY_R]) {
		myCamera.setCameraPosition(glm::vec3(9.5f, 0.0f, 9.5f));
		myCamera.setCameraTarget(glm::vec3(0.0f, 0.0f, 0.0f));
		myCamera.t = 0;
	}

	if (pressedKeys[GLFW_KEY_P]) {
		appear = 1;
	}

	if (pressedKeys[GLFW_KEY_O]) {
		appear = 0;
	}

	if (pressedKeys[GLFW_KEY_1]) {
		chase = 1;
	}

	if (pressedKeys[GLFW_KEY_2]) {
		chase = 0;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		difficulty = 0.9f;
	}

	myCamera.move(gps::STAY, cameraSpeed * time, floorx);
	
	myShip.move(gps::Model3D::MOVE_DIAGONALLY, time, position);
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels()
{
	myModel = gps::Model3D("objects/nanosuit/nanosuit.obj", "objects/nanosuit/");
	myModel.objectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	myModel.objectDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	myGround = gps::Model3D("objects/ground/ground.obj", "objects/ground/");
	myLeftSide = gps::Model3D("objects/fence/Fenceo/fence.obj", "objects/fence/Fenceo/");
	myBackSide = gps::Model3D("objects/fence/Fenceo/fence.obj", "objects/fence/Fenceo/");
	myRightSide = gps::Model3D("objects/fence/Fenceo/fence.obj", "objects/fence/Fenceo/");
	myDownSide = gps::Model3D("objects/downSide/ground.obj", "objects/downSide/");
	myAlien = gps::Model3D("objects/alien/Ork-1.obj", "objects/alien/");
	myAlien.objectPosition = glm::vec3(9.0f, 0.0f, -9.0f);	
	myAlien.objectDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	myAlien2 = gps::Model3D("objects/alien/Ork-1.obj", "objects/alien/");
	myAlien2.objectPosition = glm::vec3(-9.0f, 0.0f, -9.0f);
	myAlien2.objectDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	myMonster = gps::Model3D("objects/monster/Femuto.obj", "objects/monster/");
	myPlanet = gps::Model3D("objects/planet/globe.obj", "objects/planet/");
	myPlanet2 = gps::Model3D("objects/planet2/jupiter.obj", "objects/planet2/");
	myShip = gps::Model3D("objects/ship/SciFi_Fighter_AK5.obj", "objects/ship/");
	myRock = gps::Model3D("objects/rock/rocks_01_model.obj", "objects/rock/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	
	myShadowShader.loadShader("shaders/myShaderShadows.vert", "shaders/myShaderShadows.frag");

	mySkyBoxShader.loadShader("shaders/mySkyBox.vert", "shaders/mySkyBox.frag");

	myPlanetShader.loadShader("shaderS/myPlanet.vert", "shaders/myPlanet.frag");

	myRockShader.loadShader("shaderS/myRock.vert", "shaders/myRock.frag");

}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	myCustomShader.sendMat4(model, "model");
	
	view = myCamera.getViewMatrix();
	myCustomShader.sendMat4(view, "view");
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	myCustomShader.sendMat3(normalMatrix, "normalMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)glWindowWidth / (float)glWindowHeight, 0.1f, 1000.0f);
	myCustomShader.sendMat4(projection, "projection");
	
	//set the light direction (direction towards the light)
	myCustomShader.sendVec3(myLight.direction, "myLight.direction");
	
	//set light color
	myCustomShader.sendVec3(myLight.color, "myLight.color");
	
	myCustomShader.sendVec3(myLight.ambient, "myLight.ambient");
	
	myCustomShader.sendVec3(myLight.diffuse, "myLight.diffuse");
	
	myCustomShader.sendVec3(myLight.specular, "myLight.specular");
	
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	myCustomShader.sendMat3(lightDirMatrix, "lightDirMatrix");
	
	myCustomShader.sendMat4(computeLightSpaceMatrix(), "lightSpaceMatrix");

	myShadowShader.useShaderProgram();
	myShadowShader.sendMat4(computeLightSpaceMatrix(), "lightSpaceMatrix");

	myShadowShader.sendMat4(model, "model");

	//send point light
	myCustomShader.useShaderProgram();

	myCustomShader.sendVec3(myPointLight.position, "myPointLight.position");
	myCustomShader.sendVec3(myPointLight.ambient, "myPointLight.ambient");
	myCustomShader.sendVec3(myPointLight.diffuse, "myPointLight.diffuse");
	myCustomShader.sendVec3(myPointLight.specular, "myPointLight.specular");
	myCustomShader.sendVec3(myPointLight.color, "myPointLight.color");
	myCustomShader.sendFloat(myPointLight.constant, "myPointLight.constant");
	myCustomShader.sendFloat(myPointLight.linear, "myPointLight.linear");
	myCustomShader.sendFloat(myPointLight.quadratic, "myPointLight.quadratic");

	//send fog
	myCustomShader.sendVec3(myFog.position, "myFog.position");
	myCustomShader.sendVec4(myFog.color, "myFog.color");
	myCustomShader.sendFloat(myFog.density, "myFog.density");

	//send fog dark
	myCustomShader.sendVec3(myFogDark.position, "myFogDark.position");
	myCustomShader.sendVec4(myFogDark.color, "myFogDark.color");
	myCustomShader.sendFloat(myFogDark.density, "myFogDark.density");



	//send skybox
	mySkyBoxShader.useShaderProgram();

	view = myCamera.getViewMatrix();
	mySkyBoxShader.sendMat4(view, "view");

	projection = glm::perspective(glm::radians(45.0f), (float)glWindowWidth / (float)glWindowHeight, 0.1f, 1000.0f);
	mySkyBoxShader.sendMat4(projection, "projection");

	//send planet
	myPlanetShader.useShaderProgram();

	myPlanetShader.sendMat4(model, "model");
	myPlanetShader.sendMat4(projection, "projection");
	myPlanetShader.sendMat4(view, "view");

	//send rock
	myRockShader.useShaderProgram();

	myRockShader.sendMat4(model, "model");
	myRockShader.sendMat4(projection, "projection");
	myRockShader.sendMat4(view, "view");
}

void facesSkyBox(){
	std::vector<const GLchar*> faces;
	faces.push_back("skymap/ame_purple/purplevalley_rt.tga");
	faces.push_back("skymap/ame_purple/purplevalley_lf.tga");
	faces.push_back("skymap/ame_purple/purplevalley_up.tga");
	faces.push_back("skymap/ame_purple/purplevalley_dn.tga");
	faces.push_back("skymap/ame_purple/purplevalley_bk.tga");
	faces.push_back("skymap/ame_purple/purplevalley_ft.tga");
	mySkyBox.Load(faces);
}


void initBuffer(){
	
	glGenFramebuffers(1, &depthMapFBO);

	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

glm::mat4 computeLightSpaceMatrix(){
	float near_plane = 0.1f, far_plane = 30.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(myLight.direction,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));


	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	return lightSpaceMatrix;
}


GLfloat lightAngle = 0.5f;
void renderScene()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	currentFrameTime = glfwGetTime();
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;
	processMovement(deltaTime, position);//position of the ship


	myCustomShader.useShaderProgram();
	

	myPointLight.position = glm::rotate(glm::radians(lightAngle), glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(myPointLight.position, 1.f);
	myCustomShader.sendVec3(myPointLight.position, "myPointLight.position");


	//initialize the view matrix
	view = myCamera.getViewMatrix();
	//send view matrix data to shader	
	myCustomShader.sendMat4(view, "view");

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	myShadowShader.useShaderProgram();

	//nanosuit
	//initialize the model matrix
	model = glm::mat4(1.0f);


	if (chase == 1){
		glm::vec3 chasevector = glm::normalize(myCamera.getCameraPosition() - myModel.objectPosition);

		myModel.objectPosition = myModel.objectPosition + chasevector * deltaTime * difficulty;
		chasemodel = glm::translate(glm::mat4(1.f), myModel.objectPosition);

		if (myCamera.getCameraPosition().x >= 0.0f){
			float rotAngle = glm::acos(myModel.rotate(myModel.getDir(), -myCamera.getCameraDirection()));
			model = glm::rotate(chasemodel, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
			if (myCamera.getCameraPosition().x < 0.0f){
				float rotAngle = glm::acos(myModel.rotate(myModel.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(chasemodel, -rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			}
	}
	else{
		model = glm::translate(glm::mat4(1.f), myModel.objectPosition);

		if (myCamera.getCameraPosition().x >= 0.0f){
			float rotAngle = glm::acos(myModel.rotate(myModel.getDir(), -myCamera.getCameraDirection()));
			model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
			if (myCamera.getCameraPosition().x < 0.0f){
				float rotAngle = glm::acos(myModel.rotate(myModel.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(model, -rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			}
	}
	myShadowShader.sendMat4(model, "model");
	
	myModel.Draw(myShadowShader);

	//alien
	glm::vec3 nowPosA1S = myCamera.getCameraPosition();
	if (nowPosA1S.x > 0.0f){
		if (chase == 1){
			glm::vec3 chasea = glm::normalize(myCamera.getCameraPosition() - myAlien.objectPosition);

			myAlien.objectPosition = myAlien.objectPosition + chasea * deltaTime * difficulty;
			chasemodelA = glm::translate(glm::mat4(1.f), myAlien.objectPosition);

			if (myCamera.getCameraPosition().x >= 0.0f){
				float rotAngleA = glm::acos(myAlien.rotate(myAlien.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(chasemodelA, rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				if (myCamera.getCameraPosition().x < 0.0f){
					float rotAngleA = glm::acos(myAlien.rotate(myAlien.getDir(), -myCamera.getCameraDirection()));
					model = glm::rotate(chasemodelA, -rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
				}
		}
		else{
			model = glm::translate(glm::mat4(1.f), myAlien.objectPosition);

			if (myCamera.getCameraPosition().x >= 0.0f){
				float rotAngleA = glm::acos(myAlien.rotate(myAlien.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(model, rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				if (myCamera.getCameraPosition().x < 0.0f){
					float rotAngleA = glm::acos(myAlien.rotate(myAlien.getDir(), -myCamera.getCameraDirection()));
					model = glm::rotate(model, -rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
				}
		}
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		myCustomShader.sendMat4(model, "model");
		myAlien.objectDirection = glm::vec3(-1.0f, 0.0f, -1.0f);
		myAlien.Draw(myShadowShader);
	}

	//alien2
	glm::vec3 nowPosA2S = myCamera.getCameraPosition();
	if (nowPosA2S.x > 0.0f){
		if (chase == 1){
			glm::vec3 chasea2 = glm::normalize(myCamera.getCameraPosition() - myAlien2.objectPosition);

			myAlien2.objectPosition = myAlien2.objectPosition + chasea2 * deltaTime * difficulty;
			chasemodelA = glm::translate(glm::mat4(1.f), myAlien2.objectPosition);

			if (myCamera.getCameraPosition().x >= 0.0f){
				float rotAngleA2 = glm::acos(myAlien2.rotate(myAlien2.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(chasemodelA, rotAngleA2, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				if (myCamera.getCameraPosition().x < 0.0f){
					float rotAngleA2 = glm::acos(myAlien2.rotate(myAlien2.getDir(), -myCamera.getCameraDirection()));
					model = glm::rotate(chasemodelA, -rotAngleA2, glm::vec3(0.0f, 1.0f, 0.0f));
				}
		}
		else{
			model = glm::translate(glm::mat4(1.f), myAlien2.objectPosition);

			if (myCamera.getCameraPosition().x >= 0.0f){
				float rotAngleA2 = glm::acos(myAlien2.rotate(myAlien2.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(model, rotAngleA2, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				if (myCamera.getCameraPosition().x < 0.0f){
					float rotAngleA2 = glm::acos(myAlien2.rotate(myAlien2.getDir(), -myCamera.getCameraDirection()));
					model = glm::rotate(model, -rotAngleA2, glm::vec3(0.0f, 1.0f, 0.0f));
				}
		}
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		myCustomShader.sendMat4(model, "model");
		myAlien2.objectDirection = glm::vec3(-1.0f, 0.0f, -1.0f);
		myAlien2.Draw(myShadowShader);
	}
	



	//leftside
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-floorx, floorz / 2.0f - 1.1f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(floorx, floory, floorz / 2.0f));
	
	

	myShadowShader.sendMat4(model, "model");
	myLeftSide.Draw(myShadowShader);

	// 1. first render to depth map
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	myCustomShader.useShaderProgram();

	// 2. then render scene as normal with shadow mapping (using depth map)
	glViewport(0, 0, glWindowWidth, glWindowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 4);


	//nanosuit
	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	myCustomShader.sendMat3(normalMatrix, "normalMatrix");
	
	myModel.objectDirection = glm::vec3(0.0f, 0.0f, 1.0f);

	if (chase == 1){
		glm::vec3 chasef = glm::normalize(myCamera.getCameraPosition() - myModel.objectPosition);

		if (length(myCamera.getCameraPosition() - myModel.objectPosition) < 0.5f)
			PlaySound(TEXT("nuclearexplosion.wav"), NULL, SND_SYNC);

		myModel.objectPosition = myModel.objectPosition + chasef * deltaTime * difficulty;
		chasemodel = glm::translate(glm::mat4(1.f), myModel.objectPosition);

		std::cout << currentFrameTime << std::endl;
		if (myCamera.getCameraPosition().x >= 0.0f){
			float rotAngle = glm::acos(myModel.rotate(myModel.getDir(), -myCamera.getCameraDirection()));
			model = glm::rotate(chasemodel, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
			if (myCamera.getCameraPosition().x < 0.0f){
				float rotAngle = glm::acos(myModel.rotate(myModel.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(chasemodel, -rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			}
	}
	else{
		model = glm::translate(glm::mat4(1.f), myModel.objectPosition);

		std::cout << currentFrameTime << std::endl;
		if (myCamera.getCameraPosition().x >= 0.0f){
			float rotAngle = glm::acos(myModel.rotate(myModel.getDir(), -myCamera.getCameraDirection()));
			model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
			if (myCamera.getCameraPosition().x < 0.0f){
				float rotAngle = glm::acos(myModel.rotate(myModel.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(model, -rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			}
	}

	myCustomShader.sendMat4(model, "model");

	myModel.Draw(myCustomShader);


	//alien
	myAlien.objectDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 nowPosA1 = myCamera.getCameraPosition();
	if (nowPosA1.x > 0.0f){
		if (chase == 1){
			glm::vec3 chasea = glm::normalize(myCamera.getCameraPosition() - myAlien.objectPosition);

			if (length(myCamera.getCameraPosition() - myAlien.objectPosition) < 0.5f)
				PlaySound(TEXT("nuclearexplosion.wav"), NULL, SND_SYNC);

			myAlien.objectPosition = myAlien.objectPosition + chasea * deltaTime * difficulty;
			chasemodelA = glm::translate(glm::mat4(1.f), myAlien.objectPosition);

			if (myCamera.getCameraPosition().x >= 0.0f){
				float rotAngleA = glm::acos(myAlien.rotate(myAlien.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(chasemodelA, rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				if (myCamera.getCameraPosition().x < 0.0f){
					float rotAngleA = glm::acos(myAlien.rotate(myAlien.getDir(), -myCamera.getCameraDirection()));
					model = glm::rotate(chasemodelA, -rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
				}
		}
		else{
			model = glm::translate(glm::mat4(1.f), myAlien.objectPosition);

			if (myCamera.getCameraPosition().x >= 0.0f){
				float rotAngleA = glm::acos(myAlien.rotate(myAlien.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(model, rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				if (myCamera.getCameraPosition().x < 0.0f){
					float rotAngleA = glm::acos(myAlien.rotate(myAlien.getDir(), -myCamera.getCameraDirection()));
					model = glm::rotate(model, -rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
				}
		}
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		myCustomShader.sendMat4(model, "model");
		myAlien.objectDirection = glm::vec3(-1.0f, 0.0f, -1.0f);
		myAlien.Draw(myCustomShader);
	}

	//alien2
	myAlien2.objectDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 nowPosA2 = myCamera.getCameraPosition();
	if (nowPosA2.x < 0.0f){
		if (chase == 1){
			glm::vec3 chasea = glm::normalize(myCamera.getCameraPosition() - myAlien2.objectPosition);

			if (length(myCamera.getCameraPosition() - myAlien2.objectPosition) < 0.5f)
				PlaySound(TEXT("nuclearexplosion.wav"), NULL, SND_SYNC);

			myAlien2.objectPosition = myAlien2.objectPosition + chasea * deltaTime * difficulty;
			chasemodelA2 = glm::translate(glm::mat4(1.f), myAlien2.objectPosition);

			if (myCamera.getCameraPosition().x >= 0.0f){
				float rotAngleA = glm::acos(myAlien2.rotate(myAlien2.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(chasemodelA2, rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				if (myCamera.getCameraPosition().x < 0.0f){
					float rotAngleA = glm::acos(myAlien2.rotate(myAlien2.getDir(), -myCamera.getCameraDirection()));
					model = glm::rotate(chasemodelA2, -rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
				}
		}
		else{
			model = glm::translate(glm::mat4(1.f), myAlien2.objectPosition);

			if (myCamera.getCameraPosition().x >= 0.0f){
				float rotAngleA = glm::acos(myAlien2.rotate(myAlien2.getDir(), -myCamera.getCameraDirection()));
				model = glm::rotate(model, rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else
				if (myCamera.getCameraPosition().x < 0.0f){
					float rotAngleA = glm::acos(myAlien2.rotate(myAlien2.getDir(), -myCamera.getCameraDirection()));
					model = glm::rotate(model, -rotAngleA, glm::vec3(0.0f, 1.0f, 0.0f));
				}
		}
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		myCustomShader.sendMat4(model, "model");
		myAlien2.objectDirection = glm::vec3(-1.0f, 0.0f, -1.0f);
		myAlien2.Draw(myCustomShader);
	}

	//monster
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f, 0.0f, -4.5f));

	glm::vec3 nowPos = myCamera.getCameraPosition();

	myMonster.objectDirection = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 nowPosM = myCamera.getCameraPosition();

	if (nowPosM.x < -4.5f + 0.5f && nowPosM.x > -4.5f - 0.5f && nowPosM.z < -4.5f + 0.5f && nowPosM.z > -4.5f - 0.5f){
		PlaySound(TEXT("nuclearexplosion.wav"), NULL, SND_SYNC);
	}

	if (myCamera.getCameraPosition().x + 4.5f >= 0.0f){
		float rotAngle1 = glm::acos(myMonster.rotate(myMonster.getDir(), -myCamera.getCameraDirection()));
		model = glm::rotate(model, rotAngle1, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
		if (myCamera.getCameraPosition().x + 4.5f < 0.0f){
			float rotAngle1 = glm::acos(myMonster.rotate(myMonster.getDir(), -myCamera.getCameraDirection()));
			model = glm::rotate(model, -rotAngle1, glm::vec3(0.0f, 1.0f, 0.0f));
		}


	myCustomShader.sendMat4(model, "model");
	myCustomShader.sendVec3(myCamera.getCameraPosition(), "cameraPosition");
	if ((nowPos.x <= -0.5f && nowPos.z <= -0.5f) || t == 1){
		myMonster.Draw(myCustomShader);
		t = 1;
	}


	myPlanetShader.useShaderProgram();
	myPlanetShader.sendMat4(view, "view");
	//planet
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 50.0f, -140.0f));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	angle += 0.05f;
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
	myPlanetShader.sendMat4(model, "model");
	myPlanet.Draw(myPlanetShader);


	myCustomShader.useShaderProgram();

	//planet2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 30.0f));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	myCustomShader.sendMat4(model, "model");
	myPlanet2.Draw(myCustomShader);


	//ship
	model = translate(glm::mat4(1.0f), position);
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
	myCustomShader.sendMat4(model, "model");
	myShip.Draw(myCustomShader);


	
	//rock
	model = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f, -0.2f, 4.5f));
	myCustomShader.sendMat4(model, "model");
	myRock.Draw(myCustomShader);


	myCustomShader.useShaderProgram();
	//leftside
	for (int i = -floorz; i < floorz; i = i + 4){
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-floorx, 0.0f - 0.5f, i + 1.8f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, +1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		myCustomShader.sendMat4(model, "model");
		myLeftSide.Draw(myCustomShader);
	}


	//backside
	for (int i = -floorx; i < floorx; i= i + 4){
		model = glm::translate(glm::mat4(1.0f), glm::vec3(i + 1.8f, 0.0f - 0.5f, -floorz));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		myCustomShader.sendMat4(model, "model");
		myBackSide.Draw(myCustomShader);
	}

	//rightside
	for (int i = -floorz; i < floorz; i = i + 4){
		model = glm::translate(glm::mat4(1.0f), glm::vec3(floorx, 0.0f - 0.5f, i + 1.8f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		myCustomShader.sendMat4(model, "model");
		myRightSide.Draw(myCustomShader);
	}


	//downside
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));



	myCustomShader.sendMat4(model, "model");
	myDownSide.Draw(myCustomShader);

	if (currentFrameTime > 60.0f){
		PlaySound(TEXT("yay.wav"), NULL, SND_SYNC);
	}

	if (myCamera.getCameraPosition().y < -5.0f){
		PlaySound(TEXT("nuclearexplosion.wav"), NULL, SND_SYNC);
		
	}
	

	mySkyBoxShader.useShaderProgram();
	mySkyBox.Draw(mySkyBoxShader, view, projection);
}



int main(int argc, const char * argv[]) {

	myLight = { glm::vec3(-3.0f, 3.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f) };

	myPointLight = { glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		1.0f, 0.35f, 0.44f };

	myFog = { glm::vec3(-4.5f, 1.0f, -4.5f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		0.35f};

	myFogDark = { glm::vec3(4.5f, 1.0f, 4.5f),
		glm::vec4(0.05f, 0.05f, 0.05f, 1.0f),
		0.9f };

	initOpenGLWindow();
	initOpenGLState();
	initModels();
	initBuffer();
	initShaders();
	initUniforms();

	//last
	facesSkyBox();

	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}

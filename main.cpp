//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include "glew.h"
#include "glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <chrono>




int glWindowWidth = 1000;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
glm::mat4 model2;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::mat4 lightRotation2;
GLfloat lightAngle2;

glm::vec3 lightDir;
glm::vec3 lightDir2;
glm::vec3 Lanterna = glm::vec3(0.0f, 8.5f, 5.5f);
glm::vec3 culoare;
GLuint lightDirLoc;
GLuint lightDirLoc2;
glm::vec3 lightColor;
GLuint lightColorLoc;
GLuint lanternaLoc;
GLuint LanternaPos;

gps::Camera myCamera(
				glm::vec3(0.0f, 10.0f, 5.5f), 
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.05f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;


gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D concert;
gps::Model3D bigCube;
gps::Model3D ochi;
gps::Model3D avion;
gps::Model3D sticla;
gps::Model3D baloane;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;
gps::Shader screenQuadShader;
gps::Shader bigCubeShader;
gps::Shader reflect;
gps::Shader secLight;

GLuint shadowMapFBO;
GLuint depthMapTexture;

glm::mat4 lightView;
const GLfloat near_plane = 0.1f, far_plane = 0.f; 
glm::mat4 lightProjection;
glm::mat4 lightSpaceTrMatrix;

gps::SkyBox mySkyBox; 
gps::Shader skyboxShader;

bool showDepthMap;

std::vector<const GLchar*> faces;

//VARIABILE PENTRU ANIMATIA DE PLOAIE


int fogen = 0;
int draw_reflect = 0;
GLfloat plane_angle = 0;
GLfloat bird_angle = 0;

int enable_animation = 0;


GLenum glCheckError_(const char *file, int line) {
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

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{

	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}
glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool mouseInit = true;
float lastX = 400, lastY = 300;
float yaw = -90.0f;
float pitch = 0;
float fov = 45.0f;
float movePlaneX = 0;
float movePlaneY = 0;
float moveBird = 0;
struct cam
{
	float x, y, z;
};
cam coords[2667];


void mouseCallback(GLFWwindow* window, double x, double y) {
	if (mouseInit)
	{
		lastX = x;
		lastY = y;
		mouseInit = false;
	}
	float xoffset = x - lastX;
	float yoffset = lastY - y;
	lastX = x;
	lastY = y;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw);
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;	
		myCustomShader.useShaderProgram();
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
		myCustomShader.useShaderProgram();
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}
	
	if (pressedKeys[GLFW_KEY_K]) {
		plane_angle += 1.0f;
		movePlaneX = 0.1f;
		movePlaneY = 0.1f;
	}
	if (pressedKeys[GLFW_KEY_P]) {
		bird_angle += 1.0f;
		moveBird += 0.0f;
	}
	if (pressedKeys[GLFW_KEY_B]) {
		myCustomShader.useShaderProgram();
		culoare = glm::vec3(1.0f, 0.0f, 1.0f);
		lanternaLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lanternaCol");
		glUniform3fv(lanternaLoc, 1, glm::value_ptr(culoare));
	}
	if (pressedKeys[GLFW_KEY_N]) {
		myCustomShader.useShaderProgram();
		culoare = glm::vec3(1.0f, 1.0f, 1.0f);
		lanternaLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lanternaCol");
		glUniform3fv(lanternaLoc, 1, glm::value_ptr(culoare));
	}

	if (pressedKeys[GLFW_KEY_1])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
	}
	if (pressedKeys[GLFW_KEY_2])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //solid
	}
	if (pressedKeys[GLFW_KEY_3])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //point
	}
	if (pressedKeys[GLFW_KEY_4])
	{
		glShadeModel(GL_FLAT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //poligon
	}
	if (pressedKeys[GLFW_KEY_5])
	{
		glShadeModel(GL_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //smooth
	}
	if (pressedKeys[GLFW_KEY_C])
	{
		enable_animation = 1;
	}
	if (pressedKeys[GLFW_KEY_V])
	{
		enable_animation = 0;
	}


	if (pressedKeys[GLFW_KEY_W]) {
		std::cout << glm::to_string(myCamera.cameraFrontDirection)<< std::endl;
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);	
	}

	if (pressedKeys[GLFW_KEY_S]) {
		std::cout << glm::to_string(myCamera.cameraFrontDirection) << std::endl;
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_A]) {
		std::cout << glm::to_string(myCamera.cameraFrontDirection) << std::endl;
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_D]) {
		std::cout << glm::to_string(myCamera.cameraFrontDirection) << std::endl;
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);		
	}
	if (pressedKeys[GLFW_KEY_Z]) {
		fogen = 1;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogen"),fogen);

	}
	if (pressedKeys[GLFW_KEY_X]) {
		fogen = 0;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogen"), fogen);

	}
	
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetScrollCallback(glWindow, scroll_callback);

	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

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

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	lightCube.LoadModel("objects/cube/cube.obj");
	concert.LoadModel("objects/proiect/teren.obj");
	ochi.LoadModel("objects/ochi/ochi.obj");
	avion.LoadModel("objects/avion/avion.obj");
	sticla.LoadModel("objects/sticle/sticla.obj");
	baloane.LoadModel("objects/sticle/baloane.obj");

	
}

void initShaders() {
	
	
	faces.push_back("textures/skybox/right.tga"); 
	faces.push_back("textures/skybox/left.tga"); 
	faces.push_back("textures/skybox/top.tga"); 
	faces.push_back("textures/skybox/bottom.tga");
	faces.push_back("textures/skybox/back.tga"); 
	faces.push_back("textures/skybox/front.tga");

	
	


	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	projection = glm::perspective(glm::radians(fov), (float)retina_width / (float)retina_height, 0.1f, 1000.0f); glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


	myCustomShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
	myCustomShader.useShaderProgram();
	reflect.loadShader("shaders/reflect.vert", "shaders/reflect.frag");
	reflect.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/newShader.vert", "shaders/newShader.frag");
	depthMapShader.useShaderProgram();
	secLight.loadShader("shaders/secLight.vert", "shaders/secLight.frag");
	secLight.useShaderProgram();
	
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(fov), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");	
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	
	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));





	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	myCustomShader.useShaderProgram();
	culoare = glm::vec3(0.9f, 0.7f, 1.0f);
	//culoare = glm::vec3(1.0f, 1.0f, 1.0f);
	lanternaLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lanternaCol");
	glUniform3fv(lanternaLoc, 1, glm::value_ptr(culoare));

	LanternaPos = glGetUniformLocation(myCustomShader.shaderProgram, "lanterna");
	glUniform3fv(LanternaPos, 1, glm::value_ptr(Lanterna));

	myCustomShader.useShaderProgram();
	





}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);
	//create depth texture for FBO 
	glGenTextures(1, &depthMapTexture); 
	glBindTexture(GL_TEXTURE_2D, depthMapTexture); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; 
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 30.0f;
	glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);

	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogen"), fogen);
	model = glm::scale(model, glm::vec3(2.0f));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	concert.Draw(shader);
	
	//myCustomShader.useShaderProgram();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	baloane.Draw(myCustomShader);
	sticla.Draw(myCustomShader);
	glDisable(GL_BLEND);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	//ground.Draw(shader);	
	
}


void renderScene() {


    depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader, false);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {



		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		reflect.useShaderProgram();
		model = glm::translate(model, glm::vec3(moveBird, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(bird_angle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));

		glUniformMatrix4fv(glGetUniformLocation(reflect.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(reflect.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(reflect.shaderProgram, "projection"), 1, GL_FALSE,
			glm::value_ptr(projection));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		normalMatrixLoc = glGetUniformLocation(reflect.shaderProgram, "normalMatrix");
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glUniformMatrix4fv(glGetUniformLocation(reflect.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));
		

		ochi.Draw(reflect);

		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogen"), fogen);

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		

		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogen"), fogen);

		model = glm::translate(model, glm::vec3(movePlaneX, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(plane_angle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		avion.Draw(myCustomShader);
		glDisable(GL_BLEND);
		
		

		
		

	


		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);

		//desenez a doua sursa de lumina cu alta culoare
		secLight.useShaderProgram();
		glUniform1i(glGetUniformLocation(secLight.shaderProgram, "fogen"), fogen);

		model = glm::mat4(1.0f);
		modelLoc = glGetUniformLocation(secLight.shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		view = myCamera.getViewMatrix();
		viewLoc = glGetUniformLocation(secLight.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		normalMatrixLoc = glGetUniformLocation(secLight.shaderProgram, "normalMatrix");
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		projection = glm::perspective(glm::radians(fov), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
		projectionLoc = glGetUniformLocation(secLight.shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(0.0f, 1.0f, 0.0f));
		lightDirLoc = glGetUniformLocation(secLight.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view* lightRotation))* lightDir));

		//set light color
		
		lightShader.useShaderProgram();
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		bigCube.Draw(lightShader);

		
		skyboxShader.useShaderProgram();
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
			glm::value_ptr(view));

		projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
			glm::value_ptr(projection));

		mySkyBox.Draw(skyboxShader, view, projection);


	}
	
	
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}


	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	std::ifstream myfile;
	std::ifstream myfile2;
	std::string vector;
	myfile.open("MiscariCamera.txt");
	myfile2.open("MiscariDirectie.txt");
	glCheckError();
	
	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();		

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
		
		if (enable_animation) {
			myfile >> myCamera.cameraPosition.x;
			myfile >> myCamera.cameraPosition.y;
			myfile >> myCamera.cameraPosition.z;

			myfile2 >> myCamera.cameraFrontDirection.x;
			myfile2 >> myCamera.cameraFrontDirection.y;
			myfile2 >> myCamera.cameraFrontDirection.z;
		}


	}


	cleanup();


	return 0;
}

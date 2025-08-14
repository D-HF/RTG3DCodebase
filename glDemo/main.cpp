#include "core.h"
#include "TextureLoader.h"
#include "ArcballCamera.h"
#include "GUClock.h"
#include "PrincipleAxes.h"
#include "shader_setup.h"
#include "helper.h"
#include "AIMesh.h"
#include "Cube.h"
#include "Scene.h"
#include "Ground.h"
#include "Camera.h"


using namespace std;
using namespace glm;


#pragma region Global variables

GUClock* g_gameClock = nullptr;

// Mouse tracking
bool				g_mouseDown = false;
double				g_prevMouseX, g_prevMouseY;

// Global Example objects
// shouldn't really be anything in here for the final submission
ArcballCamera* g_mainCamera = nullptr;
CGPrincipleAxes* g_principleAxes = nullptr;
Cube* g_cube = nullptr;
Ground* g_ground = nullptr;

GLuint g_flatColourShader;

GLuint g_texDirLightShader;
vec3 g_DLdirection = vec3(1.0f, 1.0f, 0.0f);
vec3 g_DLcolour = vec3(1.0f, 1.0f, 1.0f);
vec3 g_DLambient = vec3(0.2f, 0.2f, 0.2f);

AIMesh* g_creatureMesh = nullptr;
vec3 g_beastPos = vec3(2.0f, 0.0f, 0.0f);
vec3 g_duckPos = vec3(6.0f, 0.0f, 0.0f); //duck pos
vec3 camViewPos = vec3(0.0f, 0.0f, 0.0f); //cam vector for positioning
float camMoveSpeed = 0.1; //movement for camera speed
float duckMoveSpeed = 0.1; //movement for duck speed
float g_beastRotation = 0.0f;
float g_duckRotation = 0.0f; //duck rot
AIMesh* g_planetMesh = nullptr;
AIMesh* g_duckMesh = nullptr; //duck mesh
Camera* g_cam = nullptr; //camera
vec3 g_camViewPos = vec3(0.0f, 0.0f, 0.0f); //g_cam position
AIMesh* g_catMesh = nullptr; //cat mesh
float g_catRotation = 0.0f; //cat rotation
vec3 g_catPos = vec3(4.0f, 0.0f, 0.0f); //cat position

Scene* m_useCameraIndex;

int g_showing = 0;
int g_NumExamples = 4;

//Global Game Object
Scene* g_Scene = nullptr;

// Window size
const unsigned int g_initWidth = 800;
const unsigned int g_initHeight = 800;

#pragma endregion


// Function prototypes
void renderScene();
void updateScene();
void resizeWindow(GLFWwindow* _window, int _width, int _height);
void keyboardHandler(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
void mouseMoveHandler(GLFWwindow* _window, double _xpos, double _ypos);
void mouseButtonHandler(GLFWwindow* _window, int _button, int _action, int _mods);
void mouseScrollHandler(GLFWwindow* _window, double _xoffset, double _yoffset);
void mouseEnterHandler(GLFWwindow* _window, int _entered);


int main()
{
	//
	// 1. Initialisation
	//

	g_gameClock = new GUClock();

#pragma region OpenGL and window setup

	// Initialise glfw and setup window
	glfwInit();

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_TRUE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);

	GLFWwindow* window = glfwCreateWindow(g_initWidth, g_initHeight, "GDV5001", NULL, NULL);

	// Check window was created successfully
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Set callback functions to handle different events
	glfwSetFramebufferSizeCallback(window, resizeWindow); // resize window callback
	glfwSetKeyCallback(window, keyboardHandler); // Keyboard input callback
	glfwSetCursorPosCallback(window, mouseMoveHandler);
	glfwSetMouseButtonCallback(window, mouseButtonHandler);
	glfwSetScrollCallback(window, mouseScrollHandler);
	glfwSetCursorEnterCallback(window, mouseEnterHandler);

	// Initialise glew
	glewInit();


	// Setup window's initial size
	resizeWindow(window, g_initWidth, g_initHeight);

#pragma endregion

	// Initialise scene - geometry and shaders etc
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // setup background colour to be black
	glClearDepth(1.0f);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//
	// Setup the Example Objects
	//

	g_texDirLightShader = setupShaders(string("Assets\\Shaders\\texture-directional.vert"), string("Assets\\Shaders\\texture-directional.frag"));
	g_flatColourShader = setupShaders(string("Assets\\Shaders\\flatColour.vert"), string("Assets\\Shaders\\flatColour.frag"));

	g_mainCamera = new ArcballCamera(0.0f, 0.0f, 1.98595f, 55.0f, 1.0f, 0.1f, 500.0f);//QUICKSEND
	/*
	g_cam = new Camera();
	if (g_cam) {
		g_cam->Init(g_initWidth, g_initHeight, g_Scene);
	} attempt to make a camera, unsure of where to go from here*/


	g_principleAxes = new CGPrincipleAxes();

	g_cube = new Cube();
	g_ground = new Ground();

	g_creatureMesh = new AIMesh(string("Assets\\beast\\beast.obj"));
	if (g_creatureMesh) {
		g_creatureMesh->addTexture(string("Assets\\beast\\beast_texture.bmp"), FIF_BMP);
	}
	g_planetMesh = new AIMesh(string("Assets\\gsphere.obj"));
	if (g_planetMesh) {
		g_planetMesh->addTexture(string("Assets\\Textures\\Hodges_G_MountainRock1.jpg"), FIF_JPEG);
	}

	g_duckMesh = new AIMesh(string("Assets\\duck\\rubber_duck_toy_4k.obj"));
	if (g_duckMesh) {
		g_duckMesh->addTexture(string("Assets\\duck\\rubber_duck_toy_diff_4k.jpg"), FIF_JPEG);
	}//if duckMesh
	g_catMesh = new AIMesh(string("Assets\\Cat\\12221_cat_v1_l3.obj"));
	if (g_catMesh){
		g_catMesh->addTexture(string("Assets\\Cat\\Cat_diffuse.jpg"), FIF_JPEG);
	}//if catMesh
	//
	//Set up Scene class
	//

	g_Scene = new Scene();

	ifstream manifest;
	manifest.open("manifest.txt");

	g_Scene->Load(manifest);
	g_Scene->Init();

	manifest.close();

	//
	// Main loop
	// 

	while (!glfwWindowShouldClose(window))
	{
		updateScene();
		renderScene();						// Render into the current buffer
		glfwSwapBuffers(window);			// Displays what was just rendered (using double buffering).

		glfwPollEvents();					// Use this version when animating as fast as possible

		// update window title
		char timingString[256];
		sprintf_s(timingString, 256, "GDV5001: Average fps: %.0f; Average spf: %f", g_gameClock->averageFPS(), g_gameClock->averageSPF() / 1000.0f);
		glfwSetWindowTitle(window, timingString);
	}

	glfwTerminate();

	if (g_gameClock)
	{
		g_gameClock->stop();
		g_gameClock->reportTimingData();
	}

	return 0;
}


// renderScene - function to render the current scene
void renderScene()
{
	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 cameraTransform = g_mainCamera->projectionTransform() * g_mainCamera->viewTransform();
	mat4 cameraProjection = g_mainCamera->projectionTransform();
	mat4 cameraView = g_mainCamera->viewTransform() * translate(identity<mat4>(), -camViewPos);//QUICKSEND


#// Render principle axes - no modelling transforms so just use cameraTransform
	if (true)
	{
		// Render axes 
		glUseProgram(g_flatColourShader);
		GLint pLocation;
		Helper::SetUniformLocation(g_flatColourShader, "viewMatrix", &pLocation);
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraView);
		Helper::SetUniformLocation(g_flatColourShader, "projMatrix", &pLocation);
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraProjection);
		Helper::SetUniformLocation(g_flatColourShader, "modelMatrix", &pLocation);
		mat4 modelTransform = identity<mat4>();
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

		g_principleAxes->render();
	}//if

	switch (g_showing)
	{
		case 0:
		{
			glUseProgram(g_texDirLightShader);

			GLint pLocation;
			Helper::SetUniformLocation(g_texDirLightShader, "viewMatrix", &pLocation);
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraView);
			Helper::SetUniformLocation(g_texDirLightShader, "projMatrix", &pLocation);
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraProjection);
			Helper::SetUniformLocation(g_texDirLightShader, "texture", &pLocation);
			glUniform1i(pLocation, 0); // set to point to texture unit 0 for AIMeshes
			Helper::SetUniformLocation(g_texDirLightShader, "DIRDir", &pLocation);
			glUniform3fv(pLocation, 1, (GLfloat*)&g_DLdirection);
			Helper::SetUniformLocation(g_texDirLightShader, "DIRCol", &pLocation);
			glUniform3fv(pLocation, 1, (GLfloat*)&g_DLcolour);
			Helper::SetUniformLocation(g_texDirLightShader, "DIRAmb", &pLocation);
			glUniform3fv(pLocation, 1, (GLfloat*)&g_DLambient);

			if (g_creatureMesh) {

				// Setup transforms
				Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
				mat4 modelTransform = glm::translate(identity<mat4>(), g_beastPos) * eulerAngleY<float>(glm::radians<float>(g_beastRotation));
				glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

				g_creatureMesh->setupTextures();
				g_creatureMesh->render();
			}//if creature

			if (g_planetMesh) {

				// Setup transforms
				Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
				mat4 modelTransform = glm::translate(identity<mat4>(), vec3(4.0, 4.0, 4.0));
				glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

				g_planetMesh->setupTextures();
				g_planetMesh->render();
			}//if planet
			if (g_duckMesh) {

				// Setup transforms
				Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
				mat4 modelTransform = glm::translate(identity<mat4>(), g_duckPos) * eulerAngleY<float>(glm::radians<float>(g_duckRotation));
				glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

				g_duckMesh->setupTextures();
				g_duckMesh->render();
			}//if duck
		}//case 0
		break;

		case 1:
		{
			// Render cube 
			glUseProgram(g_flatColourShader);
			GLint pLocation;
			Helper::SetUniformLocation(g_flatColourShader, "viewMatrix", &pLocation);
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraView);
			Helper::SetUniformLocation(g_flatColourShader, "projMatrix", &pLocation);
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraProjection);
			Helper::SetUniformLocation(g_flatColourShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), vec3(2.0, 0.0, 2.0));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_cube->render();
			break;
		}//case 1
		case 2:
			g_Scene->Render();
			break;
		case 3:
		{
			// Render cube 
			glUseProgram(g_flatColourShader);
			GLint pLocation;
			Helper::SetUniformLocation(g_flatColourShader, "viewMatrix", &pLocation);
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraView);
			Helper::SetUniformLocation(g_flatColourShader, "projMatrix", &pLocation);
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraProjection);
			Helper::SetUniformLocation(g_flatColourShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), vec3(2.0, 0.0, 2.0));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_ground->render();
			
			if (g_duckMesh) {
				glUseProgram(g_texDirLightShader); 
				GLint pLocation;
				Helper::SetUniformLocation(g_texDirLightShader, "viewMatrix", &pLocation);
				glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraView);
				Helper::SetUniformLocation(g_texDirLightShader, "projMatrix", &pLocation);
				glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraProjection);
				Helper::SetUniformLocation(g_texDirLightShader, "texture", &pLocation);
				glUniform1i(pLocation, 0); // set to point to texture unit 0 for AIMeshes
				Helper::SetUniformLocation(g_texDirLightShader, "DIRDir", &pLocation);
				glUniform3fv(pLocation, 1, (GLfloat*)&g_DLdirection);
				Helper::SetUniformLocation(g_texDirLightShader, "DIRCol", &pLocation);
				glUniform3fv(pLocation, 1, (GLfloat*)&g_DLcolour);
				Helper::SetUniformLocation(g_texDirLightShader, "DIRAmb", &pLocation);
				glUniform3fv(pLocation, 1, (GLfloat*)&g_DLambient);

				Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
				mat4 modelTransform = glm::translate(identity<mat4>(), g_duckPos) * eulerAngleY<float>(glm::radians<float>(g_duckRotation));
				glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

				g_duckMesh->setupTextures();
				g_duckMesh->render();
			}//if duck

			//attempt at adding cat mesh
			/*if (g_catMesh) {
				Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
				mat4 modelTransform = glm::translate(identity<mat4>(), g_catPos) * eulerAngleY<float>(glm::radians<float>(g_catRotation));
				glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

				g_catMesh->setupTextures();
				g_catMesh->render();
			}*/
			//set camera to ducks position
			camViewPos.x = g_duckPos.x;
			camViewPos.z = g_duckPos.z;

			break;

		}//case 3
	}
}


// Function called to animate elements in the scene
void updateScene() 
{
	float tDelta = 0.0f;

	if (g_gameClock) {

		g_gameClock->tick();
		tDelta = (float)g_gameClock->gameTimeDelta();
	}

	g_Scene->Update(tDelta);
}


#pragma region Event handler functions
//none of this is currently passed to the Game object
//probably a good idea to do that

// Function to call when window resized
void resizeWindow(GLFWwindow* _window, int _width, int _height)
{
	if (g_mainCamera) {

		g_mainCamera->setAspect((float)_width / (float)_height);
	}

	glViewport(0, 0, _width, _height);		// Draw into entire window
}


// Function to call to handle keyboard input
void keyboardHandler(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
	if (_action == GLFW_PRESS) {
		// check which key was pressed...
		switch (_key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(_window, true);
			break;

		case GLFW_KEY_SPACE:
			g_showing++;
			g_showing = g_showing % g_NumExamples;
			break;

		case GLFW_KEY_O:
			//reset camera
			camViewPos.x = 0.0f;
			camViewPos.y = 0.0f;
			camViewPos.z = 0.0f;
			cout << "O pressed\n";
			break;
		case GLFW_KEY_PERIOD:
			//next camera
			break;
			//Camera Movement WASD + QE
		case GLFW_KEY_W: //QUICKSEND
			//move forward
			cout << "W pressed\n";
			camViewPos.z -= camMoveSpeed;
			cout << camViewPos.z;
			break;

		case GLFW_KEY_S:
			//move backward
			camViewPos.z += camMoveSpeed;
			cout << "S pressed\n";
			cout << camViewPos.z;
			break;

		case GLFW_KEY_A:
			//move left
			camViewPos.x -= camMoveSpeed;
			cout << "A pressed\n";
			cout << camViewPos.x;
			break;

		case GLFW_KEY_D:
			//move right
			camViewPos.x += camMoveSpeed;
			cout << "D pressed\n";
			cout << camViewPos.x;
			break;
		case GLFW_KEY_Q:
			//move up camera
			camViewPos.y += camMoveSpeed;
			cout << "Q pressed\n";
			cout << camViewPos.y;
			break;
		case GLFW_KEY_E:
			//move down camera
			camViewPos.y -= camMoveSpeed;
			cout << "E pressed\n";
			cout << camViewPos.y;
			break;


		//Move duck UP/DOWN/LEFT/RIGHT
		case GLFW_KEY_UP:
			//move duck forward
			if (g_showing == 3) {
				g_duckPos.z -= duckMoveSpeed;
				cout << "\n";
				cout << g_duckPos.z;
				g_duckRotation = 180;
			}//if
			break;

		case GLFW_KEY_DOWN:
			//move duck backward
			if (g_showing == 3) {
				g_duckPos.z += duckMoveSpeed;
				cout << "\n";
				cout << g_duckPos.z;
				g_duckRotation = 0;
			}//if
			break;

		case GLFW_KEY_LEFT:
			//move duck left
			if (g_showing == 3) {
				g_duckPos.x -= duckMoveSpeed;
				cout << "\n";
				cout << g_duckPos.x;
				g_duckRotation = 270;
			}//if
			break;

		case GLFW_KEY_RIGHT:
			//move duck right			
			if (g_showing == 3) {
				g_duckPos.x += duckMoveSpeed;
				cout << "\n";
				cout << g_duckPos.x;
				g_duckRotation = 90;
			}
			break;

		default:
			{
			}//default
		}//switch
	}
	else if (_action == GLFW_RELEASE) 
	{
		// handle key release events
		switch (_key)
		{
		default:
		{
		}
		}
	}
	else if (_action == GLFW_REPEAT) {
		switch (_key) {

			//move camera WASD + QE
		case GLFW_KEY_W:
			//move forward
				if (g_showing != 3) {
					cout << "W pressed\n";
					camViewPos.z -= camMoveSpeed;
					cout << camViewPos.z;
				}//if
				break;

			case GLFW_KEY_S:
				//move backward
				if (g_showing != 3) {
					camViewPos.z += camMoveSpeed;
					cout << "S pressed\n";
					cout << camViewPos.z;
				}//if
				break;

			case GLFW_KEY_A:
				//move left
				if (g_showing != 3) {
					camViewPos.x -= camMoveSpeed;
					cout << "A pressed\n";
					cout << camViewPos.x;
				}//if
				break;

			case GLFW_KEY_D:
				//move right
				if (g_showing != 3) {
					camViewPos.x += camMoveSpeed;
					cout << "D pressed\n";
					cout << camViewPos.x;
				}//if
				break;

			case GLFW_KEY_Q:
				//move up camera
				if (g_showing != 3) {
					camViewPos.y += camMoveSpeed;
					cout << "Q pressed\n";
					cout << camViewPos.y;
				}//if
				break;
			case GLFW_KEY_E:
				//move down camera
				if (g_showing != 3) {
					camViewPos.y -= camMoveSpeed;
					cout << "E pressed\n";
					cout << camViewPos.y;
				}//if
				break;

			//move duck UP/DOWN/LEFT/RIGHT
			case GLFW_KEY_UP:
				//move duck forward
				if (g_showing == 3) {
					g_duckPos.z -= duckMoveSpeed;
					cout << "\n";
					cout << g_duckPos.z;

					//move camera
					cout << "W pressed\n";
					camViewPos.z -= camMoveSpeed;
					cout << camViewPos.z;
				}//if
				break;

			case GLFW_KEY_DOWN:
				//move duck backward
				if (g_showing == 3) {
					g_duckPos.z += duckMoveSpeed;
					cout << "\n";
					cout << g_duckPos.z;

					//move camera
					camViewPos.z += camMoveSpeed;
					cout << "S pressed\n";
					cout << camViewPos.z;
				}//if
				break;

			case GLFW_KEY_LEFT:
				//move duck left
				if (g_showing == 3) {
					g_duckPos.x -= duckMoveSpeed;
					cout << "\n";
					cout << g_duckPos.x;

					//move camera
					camViewPos.x -= camMoveSpeed;
					cout << "A pressed\n";
					cout << camViewPos.x;
				}//if
				break;

			case GLFW_KEY_RIGHT:
				//move duck right
				if (g_showing == 3) {
					g_duckPos.x += duckMoveSpeed;
					cout << "\n";
					cout << g_duckPos.x;

					//move camera 
					camViewPos.x += camMoveSpeed;
					cout << "D pressed\n";
					cout << camViewPos.x;
				}//if
				break;
			default:
			{
			}//default
		}//switch
	}//else if
}


void mouseMoveHandler(GLFWwindow* _window, double _xpos, double _ypos) 
{
	if (g_mouseDown) {

		//float tDelta = gameClock->gameTimeDelta();

		float dx = float(_xpos - g_prevMouseX);// *360.0f * tDelta;
		float dy = float(_ypos - g_prevMouseY);// *360.0f * tDelta;

		if (g_mainCamera)
			g_mainCamera->rotateCamera(-dy, -dx);

		g_prevMouseX = _xpos;
		g_prevMouseY = _ypos;
	}
}

void mouseButtonHandler(GLFWwindow* _window, int _button, int _action, int _mods) 
{
	if (_button == GLFW_MOUSE_BUTTON_LEFT) 
	{
		if (_action == GLFW_PRESS) 
		{
			g_mouseDown = true;
			glfwGetCursorPos(_window, &g_prevMouseX, &g_prevMouseY);
		}
		else if (_action == GLFW_RELEASE) 
		{
			g_mouseDown = false;
		}
	}
}

void mouseScrollHandler(GLFWwindow* _window, double _xoffset, double _yoffset) {

	if (g_mainCamera) 
	{
		if (_yoffset < 0.0)
			g_mainCamera->scaleRadius(1.1f);
		else if (_yoffset > 0.0)
			g_mainCamera->scaleRadius(0.9f);
	}
}

void mouseEnterHandler(GLFWwindow* _window, int _entered) 
{
}

#pragma endregion
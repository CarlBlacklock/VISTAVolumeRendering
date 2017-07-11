#include "..\Headers\renderThreads.h"
#include "..\Headers\DataCube.h"
#include "..\Headers\CompileShaders.h"
#include "..\Headers\Camera.h"
#include "..\Headers\Utilities.h"

//Signal used to determine if the thread should die

extern bool volumeRenderClosed;
bool GLEWINIT = false;
bool volumeRenderShouldClose = false;

//Define externals
extern unsigned char *volumeData;
extern int xResolution;
extern int yResolution;
extern int numberOfFiles;
extern std::mutex statusMutex;
extern int status;

const GLint WIDTH = 640, HEIGHT = 480;
GLFWwindow *window;
//Identity Matrix used to simplify glm operations
glm::mat4 IdentityMatrix = glm::mat4(1.0f);
glm::mat4 FrustrumMatrix;
glm::vec3 viewingDir;
float theta = 0.0f;
bool animate = false;
double animateStart;
glm::vec2 mousePosition = glm::vec2((float)0.0f, (float)0.0f);
GLuint crossSectionExtents;
float *extents;

DataCube* myCube;
int xMinState, xMaxState, yMinState, yMaxState, zMinState, zMaxState;
int renderMode = 0;
Camera* myCamera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);

	if (!(width == 0 || height == 0)) {
		FrustrumMatrix = glm::perspective(degreesToRadians((float)45.0f), (float)width / (float)height, 0.1f, 100.0f);
	}
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	if (action == GLFW_PRESS)
	{
		switch (key) {
		case GLFW_KEY_Z:
			if (animate) {
				animate = 0;
			}
			else {
				animate = 1;
				animateStart = glfwGetTime();
			}
			break;
		case GLFW_KEY_Q:
			//Increase the max x extent
			if (extents[1] == (float)xResolution) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[1] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;
		case GLFW_KEY_A:
			//Decrease the max x extent
			if (extents[1] == 0.0f || extents[0] == extents[1] + 1.0f) {
				//Do nothing
			}
			else {
				extents[1] -= 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_S:
			//Decrease the min x extent
			if (extents[0] == (float)xResolution || extents[0] == extents[1] + 1.0f) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[0] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;
		case GLFW_KEY_W:
			//Increase the min x extent
			if (extents[0] == 0.0f) {
				//Do nothing
			}
			else {
				extents[0] -= 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;
		case GLFW_KEY_E:
			//Increase the max x extent
			if (extents[3] == (float)yResolution) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[3] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;
		case GLFW_KEY_D:
			//Decrease the max x extent
			if (extents[3] == 0.0f || extents[2] == extents[3] + 1.0f) {
				//Do nothing
			}
			else {
				extents[3] -= 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_F:
			//Decrease the min x extent
			if (extents[2] == (float)yResolution || extents[2] == extents[3] + 1.0f) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[2] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;
		case GLFW_KEY_R:
			//Increase the min x extent
			if (extents[2] == 0.0f) {
				//Do nothing
			}
			else {
				extents[2] -= 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_T:
			//Increase the max x extent
			if (extents[5] == (float)numberOfFiles) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[5] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;
		case GLFW_KEY_G:
			//Decrease the max x extent
			if (extents[5] == 0.0f || extents[4] == extents[5] + 1.0f) {
				//Do nothing
			}
			else {
				extents[5] -= 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_H:
			//Decrease the min x extent
			if (extents[4] == (float)numberOfFiles || extents[4] == extents[5] + 1.0f) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[4] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;
		case GLFW_KEY_Y:
			//Increase the min x extent
			if (extents[4] == 0.0f) {
				//Do nothing
			}
			else {
				extents[4] -= 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xResolution, (extents[3] - extents[2]) / (float)yResolution, (extents[5] - extents[4]) / (float)numberOfFiles));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_B:
			renderMode = 0;
			break;

		case GLFW_KEY_N:
			renderMode = 1;
			break;

		case GLFW_KEY_P:
			extents[0] = 0.0f;
			extents[1] = (float)xResolution;
			extents[2] = 0.0f;
			extents[3] = (float)yResolution;
			extents[4] = 0.0f;
			extents[5] = (float)numberOfFiles;
			myCube->ResetModelMatrix();
			break;

		case GLFW_KEY_C:
			double tempX, tempY;
			glfwGetCursorPos(window, &tempX, &tempY);
			if (tempX != NULL && tempY != NULL) {
				//glfwGetCursorPos sets coordinates to NULL when they are undefined
				//Update stored mouse position
				mousePosition = glm::vec2((float)tempX, (float)tempY);
				std::cout << "Mouse Position set to: " << tempX << ", " << tempY << std::endl;
			}
			break;

		case GLFW_KEY_LEFT:
			myCamera->moveCamera(0.0f, 0.01f * M_PI);
			break;
		case GLFW_KEY_RIGHT:
			myCamera->moveCamera(0.0f, -0.01f * M_PI);
			break;
		case GLFW_KEY_DOWN:
			myCamera->moveCamera(0.01f * M_PI, 0.0f);
			break;
		case GLFW_KEY_UP:
			myCamera->moveCamera(-0.01f * M_PI, 0.0f);
			break;

		case GLFW_KEY_V:
			myCamera->ResetCamera();
			break;
		}
	}
}

void renderVolume() {
	int i;
	
	window = CreateWindow(WIDTH, HEIGHT, "Rendering", nullptr, nullptr);
	int screenWidth, screenHeight;

	if (nullptr == window) {
		std::cout << "Failed to create GLFW window: in volume" << std::endl;

		volumeRenderClosed = true;
		return;
	}
	
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;


		return;
	}
	GLEWINIT = true;
	glViewport(0, 0, screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	//Enable depth testing and set depth function to less than or equal
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//Use counterclock wise winding and enable back face culling -- Disabled for now
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	/*
	//Place camera at 0,0,0
	//glm::vec3 LookFrom = glm::vec3((float)cos(theta), 0.0f, (float)sin(theta));
	glm::vec3 LookFrom = glm::vec3(1.0f, 0.0f, 0.0f);
	//glm::vec3(0.7071067f, 0.0f, 0.7071067f);
	glm::mat4 ViewMatrix = glm::lookAt(LookFrom, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	*/
	//Set up view matrix and frustrum matrix
	glm::mat4 OrthoMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.10f, 100.0f);
	FrustrumMatrix = glm::perspective(degreesToRadians((float)45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);


	//Create a light
	Light myLight;
	myLight.intensity = glm::vec3(0.60f, 0.60f, 0.60f);

	myLight.position = glm::vec3((float)cos(theta - 0.785398f), 0.0f, (float)sin(theta - 0.785398f));

	GLuint OrthoRayCastingProgram = CompileShaders("../Shaders/OrthoRayCastingVertex.vs", "../Shaders/OrthoRayCastingFragment.fs");
	GLuint OrthoMIPProgram = CompileShaders("../Shaders/OrthoMIPVertex.vs", "../Shaders/OrthoMIPFragment.fs");
	GLuint MovingOrthoRayCastingProgram = CompileShaders("../Shaders/MovingOrthoRayCastingVertex.vs", "../Shaders/MovingOrthoRayCastingFragment.fs");
	GLuint MovingPerspectiveRayCastingProgram = CompileShaders("../Shaders/MovingPerspectiveRayCastingVertex.vs", "../Shaders/MovingPerspectiveRayCastingFragment.fs");
	GLuint MovingOrthoMIPProgram = CompileShaders("../Shaders/MovingOrthoMIPVertex.vs", "../Shaders/MovingOrthoMIPFragment.fs");
	GLuint ComputeShaderProgram = CompileComputeShader("../Shaders/basicCompute.comp");

	myCube = new DataCube();
	myCamera = new Camera();

	//glm::mat4 RotationMatrix = glm::rotate(IdentityMatrix, degreesToRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//myCube.MultiplyModelMatrix(RotationMatrix);

	GLuint volumeID;
	glGenTextures(1, &volumeID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeID);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, xResolution, yResolution, numberOfFiles, 0, GL_RED, GL_UNSIGNED_BYTE, volumeData);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	statusMutex.lock();
	status++;
	statusMutex.unlock();


	GLuint gradientID;
	calculateGradients(ComputeShaderProgram, xResolution, yResolution, numberOfFiles, &gradientID);

	viewingDir = glm::vec3(0.0, 0.0, 0.0) - myCamera->LookFrom;
	viewingDir = glm::normalize(viewingDir);
	glm::vec3 Resolution = glm::vec3((float)xResolution, (float)yResolution, (float)numberOfFiles);

	//Initialize extents
	extents = (float *)malloc(6 * sizeof(float));
	extents[0] = 0.0f;
	extents[1] = (float)xResolution;
	extents[2] = 0.0f;
	extents[3] = (float)yResolution;
	extents[4] = 0.0f;
	extents[5] = (float)numberOfFiles;

	//Start the rendering
	int numberOfFrames = 0;
	char title[512];
	double currentTime = glfwGetTime();
	while (!(glfwWindowShouldClose(window) || volumeRenderShouldClose)) {


		glfwPollEvents();
		/*
		if (animate) {
		theta = theta + (glfwGetTime() - animateStart) * 0.1;
		LookFrom = glm::vec3((float)cos(theta), 0.0f, (float)sin(theta));
		myLight.position = glm::vec3((float)cos(theta - 0.785398f), 0.0f, (float)sin(theta - 0.785398f));
		ViewMatrix = glm::lookAt(LookFrom, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		viewingDir = glm::vec3(0.0, 0.0, 0.0) - LookFrom;
		viewingDir = glm::normalize(viewingDir);
		animateStart = glfwGetTime();
		}
		*/

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (renderMode == 1) {
			myCube->DrawDataCubeOrthoView(MovingOrthoRayCastingProgram, volumeID, gradientID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, mousePosition, myLight, extents);
		}
		if (renderMode == 0) {
			myCube->DrawDataCubeOrthoView(MovingOrthoMIPProgram, volumeID, gradientID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, mousePosition, myLight, extents);
		}
		//myCube.DrawDataCubeOrthoView(MovingPerspectiveRayCastingProgram, volumeID, gradientID, OrthoMatrix, ViewMatrix, viewingDir, Resolution, myLight);
		//myCube.DrawDataCube(OrthoMIPProgram, volumeID);
		//myCube.DrawDataCube(OrthoRayCastingProgram, volumeID);

		glfwSwapBuffers(window);
		numberOfFrames++;
		if (glfwGetTime() - currentTime >= 1.0) {
			currentTime = glfwGetTime();
			sprintf_s(title, "%s @ %d FPS", "MIP Rendering: ", numberOfFrames);
			glfwSetWindowTitle(window, title);
			numberOfFrames = 0;
		}

	}
	volumeRenderClosed = true;
	glDeleteProgram(OrthoMIPProgram);
	glDeleteProgram(OrthoRayCastingProgram);
	glDeleteProgram(MovingOrthoRayCastingProgram);
	glDeleteProgram(MovingOrthoMIPProgram);
	glDeleteProgram(ComputeShaderProgram);
	glDeleteProgram(MovingPerspectiveRayCastingProgram);
	glDeleteTextures(1, &volumeID);
	myCube->CleanUp();
	glDeleteTextures(1, &gradientID);
	free(extents);
	return;
}

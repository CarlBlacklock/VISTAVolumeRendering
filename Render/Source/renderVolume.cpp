#include "..\Headers\renderThreads.h"
#include "..\Headers\DataCube.h"
#include "..\Headers\CompileShaders.h"
#include "..\Headers\Camera.h"
#include "..\Headers\Utilities.h"
#include "..\Headers\ScreenFillingQuad.h"
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
extern glm::vec3 focus;
extern std::mutex focusLock;
extern bool focusChanged;

extern glm::vec3 probeFocus;
extern std::mutex probeFocusLock;
extern bool probeFocusChanged;

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
float cursorVolumePosition[3] = { 0.0f, 0.0f, 0.0f };;

GLuint fboHandle, colorTex, posTex, depthTex;
bool fboTexturesGenerated = false;

DataCube* myCube;
int xMinState, xMaxState, yMinState, yMaxState, zMinState, zMaxState;
int renderMode = 0;
int filterMode = 0;
Camera* myCamera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);

	if (!(width == 0 || height == 0)) {
		FrustrumMatrix = glm::perspective(degreesToRadians((float)45.0f), (float)width / (float)height, 0.1f, 100.0f);
		if (fboTexturesGenerated) {
			glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

			glBindTexture(GL_TEXTURE_2D, colorTex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

			glBindTexture(GL_TEXTURE_2D, posTex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		}
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (fboTexturesGenerated && renderMode == 0 && focusLock.try_lock()) {
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glReadPixels((GLint)xpos, (GLint)ypos, 1, 1, GL_RGB, GL_FLOAT, cursorVolumePosition);
		//focus = glm::vec3(cursorVolumePosition[0], cursorVolumePosition[1], cursorVolumePosition[2]);
		focus = glm::vec3(cursorVolumePosition[0], cursorVolumePosition[1], cursorVolumePosition[2]);
		//focusChanged = true;
		focusLock.unlock();
		//std::cout << "X pos: " << std::setprecision(15) << cursorVolumePosition[0] << " Y pos: " << cursorVolumePosition[1] << " Z pos: " << cursorVolumePosition[2];
	}
	if (fboTexturesGenerated && renderMode == 0 && probeFocusLock.try_lock()) {
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glReadPixels((GLint)xpos, (GLint)ypos, 1, 1, GL_RGB, GL_FLOAT, cursorVolumePosition);
		//focus = glm::vec3(cursorVolumePosition[0], cursorVolumePosition[1], cursorVolumePosition[2]);
		probeFocus = glm::vec3(cursorVolumePosition[0], cursorVolumePosition[1], cursorVolumePosition[2]);
		probeFocusChanged = true;
		probeFocusLock.unlock();
		//std::cout << "X pos: " << std::setprecision(15) << cursorVolumePosition[0] << " Y pos: " << cursorVolumePosition[1] << " Z pos: " << cursorVolumePosition[2];
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (fboTexturesGenerated && renderMode == 0 && focusLock.try_lock()) {
			glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			focus = glm::vec3(cursorVolumePosition[0], cursorVolumePosition[1], cursorVolumePosition[2]);
			//focusChanged = true;
			focusLock.unlock();
			//std::cout << "X pos: " << std::setprecision(15) << cursorVolumePosition[0] << " Y pos: " << cursorVolumePosition[1] << " Z pos: " << cursorVolumePosition[2] << std::endl;
			focusChanged = true;
		}
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

		case GLFW_KEY_Z:
			filterMode = 0;
			break;

		case GLFW_KEY_X:
			filterMode = 1;
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
				std::cout << "Mouse Position set to: " << std::setprecision(15) << tempX << ", " << tempY << std::endl;
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
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
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
	glm::mat4 OrthoMatrix = glm::ortho(-.750f, 0.750f, -0.750f, 0.750f, 0.10f, 100.0f);
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
	GLuint sobelGaussFilterProgram = CompileComputeShader("../Shaders/sobelGaussFilter.comp");
	GLuint alternativeSobelGaussProgram = CompileComputeShader("../Shaders/alternateSobelGauss.comp");
	GLuint ScreenFillingQuadProgram = CompileShaders("../Shaders/test.vs", "../Shaders/test.fs");

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

	GLuint sobelGaussFilterID;
	sobelGaussFilter(sobelGaussFilterProgram, xResolution, yResolution, numberOfFiles, &sobelGaussFilterID);

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

	//Generate fbo
	glGenTextures(1, &colorTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &posTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, posTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &depthTex);
	glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);

	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, posTex, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTex);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	fboTexturesGenerated = true;

	//Create screen filling quad
	ScreenFillingQuad myQuad = ScreenFillingQuad();
	//Start the rendering
	int numberOfFrames = 0;
	char title[512];
	double currentTime = glfwGetTime();
	float alpha = 1.0f;
	float beta = 0.0f;
	while (!(glfwWindowShouldClose(window) || volumeRenderShouldClose)) {


		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (renderMode == 1) {
			if (filterMode == 0) {
				myCube->DrawDataCubeOrthoView(MovingOrthoRayCastingProgram, volumeID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, mousePosition, extents, alpha, beta, filterMode);
			}
			else if (filterMode == 1) {
				myCube->DrawDataCubeOrthoView(MovingOrthoRayCastingProgram, sobelGaussFilterID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, mousePosition, extents, alpha, beta, filterMode);
			}
		}
		if (renderMode == 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
			glDrawBuffers(2, drawBuffers);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (filterMode == 0) {
				myCube->DrawDataCubeOrthoView(MovingOrthoMIPProgram, volumeID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, mousePosition, extents, alpha, beta, filterMode);
			}
			else if (filterMode == 1) {
				myCube->DrawDataCubeOrthoView(MovingOrthoMIPProgram, sobelGaussFilterID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, mousePosition, extents, alpha, beta, filterMode);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			myQuad.DrawScreenFillingQuad(ScreenFillingQuadProgram, OrthoMatrix, myCamera->ViewMatrix, colorTex);
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
	glDeleteProgram(sobelGaussFilterProgram);
	glDeleteProgram(alternativeSobelGaussProgram);
	glDeleteTextures(1, &volumeID);
	myCube->CleanUp();
	myQuad.CleanUp();
	glDeleteProgram(ScreenFillingQuadProgram);
	glDeleteTextures(1, &colorTex);
	glDeleteTextures(1, &posTex);
	glDeleteTextures(1, &sobelGaussFilterID);
	glDeleteRenderbuffers(1, &depthTex);
	glDeleteFramebuffers(1, &fboHandle);
	glDeleteTextures(1, &gradientID);
	free(extents);
	return;
}


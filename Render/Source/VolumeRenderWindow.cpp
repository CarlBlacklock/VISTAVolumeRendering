#include "..\Headers\VolumeRenderWindow.h"

glm::mat4 IdentityMatrix = glm::mat4(1.0f);

VolumeRenderWindow::VolumeRenderWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title){
	closed = false;
	shouldClose = false;
	xRes = xResolution;
	yRes = yResolution;
	zRes = numberOfFiles;
	numOfSubdivisions = numberOfSubdivisions;
	dataLocation = volumeData;
	GLEWINIT = false;
	internalThread = std::thread(&VolumeRenderWindow::run, this, title);
}


VolumeRenderWindow::~VolumeRenderWindow(){
	if (!closedStatus()) {
		forceClose();
	}
	if (internalThread.joinable()) {
		internalThread.join();
	}
}

void VolumeRenderWindow::attachProbeWindow(localProbeWindow* probeWindow) {
	attachedProbeWindow = probeWindow;
}
void VolumeRenderWindow::attachLocalHistogramWindow(localProbeWindow* localHistogramWindow) {
	attachedLocalHistogramWindow = localHistogramWindow;
}
void VolumeRenderWindow::attachHistogramWindow(histogramWindow* histogramWindow) {
	attachedHistogramWindow = histogramWindow;
}

void renderWindowFramebufferCallbackFunc(GLFWwindow *w, int x, int y) {
	static_cast<VolumeRenderWindow*>(glfwGetWindowUserPointer(w))->framebufferCallback(w, x, y);
}

void renderWindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	static_cast<VolumeRenderWindow*>(glfwGetWindowUserPointer(window))->keyCallback(window, key, scancode, action, mods);
}

void renderWindowCursorCallback(GLFWwindow* window, double xpos, double ypos) {
	static_cast<VolumeRenderWindow*>(glfwGetWindowUserPointer(window))->VolumeRenderCursorCallBack(window, xpos, ypos);
}

void renderWindowMouseCallback(GLFWwindow* window, int button, int action, int mods) {
	static_cast<VolumeRenderWindow*>(glfwGetWindowUserPointer(window))->VolumeRenderMouseButtonCallBack(window, button, action, mods);
}

void VolumeRenderWindow::run(const char * title){
	glfwWindow = CreateWindow(640, 480, title, nullptr, nullptr);

	if (nullptr == glfwWindow) {
		std::cout << "Failed to initialize window" << std::endl;
		closed = true;
		return;
	}
	glfwGetFramebufferSize(glfwWindow, &screenWidth, &screenHeight);
	glfwMakeContextCurrent(glfwWindow);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, renderWindowFramebufferCallbackFunc);
	glViewport(0, 0, screenWidth, screenHeight);

	glfwSetKeyCallback(glfwWindow, renderWindowKeyCallback);

	glfwSetCursorPosCallback(glfwWindow, renderWindowCursorCallback);
	glfwSetMouseButtonCallback(glfwWindow, renderWindowMouseCallback);
	
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;


		return;
	}


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//Use counterclock wise winding and enable back face culling -- Disabled for now
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glm::mat4 OrthoMatrix = glm::ortho(-.750f, 0.750f, -0.750f, 0.750f, 0.10f, 100.0f);

	GLuint MovingOrthoRayCastingProgram = CompileShaders("../Shaders/MovingOrthoRayCastingVertex.vs", "../Shaders/MovingOrthoRayCastingFragment.fs");
	GLuint MovingOrthoMIPProgram = CompileShaders("../Shaders/MovingOrthoMIPVertex.vs", "../Shaders/MovingOrthoMIPFragment.fs");
	GLuint sobelGaussFilterProgram = CompileComputeShader("../Shaders/sobelGaussFilter.comp");
	GLuint ScreenFillingQuadProgram = CompileShaders("../Shaders/test.vs", "../Shaders/test.fs");

	myCube = new DataCube();
	myCamera = new Camera();

	
	glGenTextures(1, &volumeID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeID);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, xRes, yRes, zRes, 0, GL_RED, GL_UNSIGNED_BYTE, dataLocation);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	sobelGaussFilter(sobelGaussFilterProgram, xRes, yRes, zRes, &sobelGaussFilterID);
	
	glm::vec3 Resolution = glm::vec3((float)xRes, (float)yRes, (float)zRes);

	extents = (float *)malloc(6 * sizeof(float));
	extents[0] = 0.0f;
	extents[1] = (float)xRes;
	extents[2] = 0.0f;
	extents[3] = (float)yRes;
	extents[4] = 0.0f;
	extents[5] = (float)zRes;

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

	ScreenFillingQuad myQuad = ScreenFillingQuad();

	int numberOfFrames = 0;
	char newTitle[512];
	double currentTime = glfwGetTime();
	float alpha = 0.50f;
	float beta = 0.50f;
	GLEWINIT = true;
	while (!(glfwWindowShouldClose(glfwWindow) || shouldClose)) {


		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (renderMode == 1) {
			if (filterMode == 0) {
				myCube->DrawDataCubeOrthoView(MovingOrthoRayCastingProgram, volumeID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, extents, alpha, beta, filterMode);
			}
			else if (filterMode == 1) {
				myCube->DrawDataCubeOrthoView(MovingOrthoRayCastingProgram, sobelGaussFilterID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, extents, alpha, beta, filterMode);
			}
		}
		if (renderMode == 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
			glDrawBuffers(2, drawBuffers);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (filterMode == 0) {
				myCube->DrawDataCubeOrthoView(MovingOrthoMIPProgram, volumeID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, extents, alpha, beta, filterMode);
			}
			else if (filterMode == 1) {
				myCube->DrawDataCubeOrthoView(MovingOrthoMIPProgram, sobelGaussFilterID, OrthoMatrix, myCamera->ViewMatrix, myCamera->ViewDir, Resolution, extents, alpha, beta, filterMode);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			myQuad.DrawScreenFillingQuad(ScreenFillingQuadProgram, OrthoMatrix, myCamera->ViewMatrix, colorTex);
		}
		//myCube.DrawDataCubeOrthoView(MovingPerspectiveRayCastingProgram, volumeID, gradientID, OrthoMatrix, ViewMatrix, viewingDir, Resolution, myLight);
		//myCube.DrawDataCube(OrthoMIPProgram, volumeID);
		//myCube.DrawDataCube(OrthoRayCastingProgram, volumeID);

		glfwSwapBuffers(glfwWindow);
		numberOfFrames++;
		if (glfwGetTime() - currentTime >= 1.0) {
			currentTime = glfwGetTime();
			sprintf_s(newTitle, "%s @ %d FPS", title, numberOfFrames);
			glfwSetWindowTitle(glfwWindow, newTitle);
			numberOfFrames = 0;
		}

	}
	closed = true;
	glDeleteProgram(MovingOrthoRayCastingProgram);
	glDeleteProgram(MovingOrthoMIPProgram);
	glDeleteProgram(sobelGaussFilterProgram);
	glDeleteTextures(1, &volumeID);
	myCube->CleanUp();
	myQuad.CleanUp();
	glDeleteProgram(ScreenFillingQuadProgram);
	glDeleteTextures(1, &colorTex);
	glDeleteTextures(1, &posTex);
	glDeleteTextures(1, &sobelGaussFilterID);
	glDeleteRenderbuffers(1, &depthTex);
	glDeleteFramebuffers(1, &fboHandle);
	free(extents);
}



void VolumeRenderWindow::framebufferCallback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);

	if (!(width == 0 || height == 0)) {
		glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

		glBindTexture(GL_TEXTURE_2D, colorTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, posTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	}
}

void VolumeRenderWindow::VolumeRenderCursorCallBack(GLFWwindow* window, double xpos, double ypos) {
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glReadPixels((GLint)xpos, (GLint)ypos, 1, 1, GL_RGB, GL_FLOAT, cursorVolumePosition);
		//focus = glm::vec3(cursorVolumePosition[0], cursorVolumePosition[1], cursorVolumePosition[2]);
		glm::ivec3 focus = glm::ivec3((int)((float) xRes *cursorVolumePosition[0]), (int)((float)yRes*cursorVolumePosition[1]), (int)((float)zRes*cursorVolumePosition[2]));
		if (attachedProbeWindow != nullptr) {
			if (!(focus.x == 0 && focus.y == 0 && focus.z == 0)) {
				attachedProbeWindow->changeFocus(focus);
			}
		}
}

void VolumeRenderWindow::VolumeRenderMouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glm::vec3 focus = glm::vec3((int)((float)xRes *cursorVolumePosition[0]), (int)((float)yRes*cursorVolumePosition[1]), (int)((float)zRes*cursorVolumePosition[2]));
		if(attachedLocalHistogramWindow != nullptr){
			attachedLocalHistogramWindow->changeFocus(focus);
		}
	}
}

void VolumeRenderWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

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
			if (extents[1] == (float)xRes) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[1] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
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
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_S:
			//Decrease the min x extent
			if (extents[0] == (float)xRes || extents[0] == extents[1] + 1.0f) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[0] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
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
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;
		case GLFW_KEY_E:
			//Increase the max x extent
			if (extents[3] == (float)yRes) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[3] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
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
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_F:
			//Decrease the min x extent
			if (extents[2] == (float)yRes || extents[2] == extents[3] + 1.0f) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[2] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
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
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_T:
			//Increase the max x extent
			if (extents[5] == (float)zRes) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[5] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
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
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
				myCube->MultiplyModelMatrix(scaleMatrix);
			}
			break;

		case GLFW_KEY_H:
			//Decrease the min x extent
			if (extents[4] == (float)zRes || extents[4] == extents[5] + 1.0f) {
				//Can't go outside the volume do nothing
			}
			else {
				extents[4] += 1.0f;
				myCube->ResetModelMatrix();
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
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
				glm::mat4 scaleMatrix = glm::scale(IdentityMatrix, glm::vec3((extents[1] - extents[0]) / (float)xRes, (extents[3] - extents[2]) / (float)yRes, (extents[5] - extents[4]) / (float)zRes));
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
			extents[1] = (float)xRes;
			extents[2] = 0.0f;
			extents[3] = (float)yRes;
			extents[4] = 0.0f;
			extents[5] = (float)zRes;
			myCube->ResetModelMatrix();
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

bool VolumeRenderWindow::closedStatus(){
	return closed;
}

bool VolumeRenderWindow::isGLEWINIT()
{
	return GLEWINIT;
}

void VolumeRenderWindow::forceClose(){
	shouldClose = true;
}

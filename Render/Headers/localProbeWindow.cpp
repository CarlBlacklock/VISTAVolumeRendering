#include "localProbeWindow.h"



localProbeWindow::localProbeWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title, glm::vec3 initialFocus){
	closed = false;
	shouldClose = false;
	xRes = xResolution;
	yRes = yResolution;
	zRes = numberOfFiles;
	numOfSubdivisions = numberOfSubdivisions;
	dataLocation = volumeData;
	focus = initialFocus;
	//std::thread myThread(this.run, title);
	internalThread = std::thread(&localProbeWindow::run, this, title);
}

void localProbeWindow::framebufferCallback(GLFWwindow *histogramWindow, int width, int height) {
	glViewport(0, 0, width, height);
}



void localProbeWindow::forceClose() {
	shouldClose = true;
}

bool localProbeWindow::closedStatus() {
	return closed;
}
void localProbeWindow::updateSubdivisions(int newSubdivisions) {
	numOfSubdivisions = newSubdivisions;
	myHistogram.CleanUp();
	myHistogram = Histogram(focus, numOfSubdivisions, dataLocation, xRes, yRes, zRes);
}

void localProbeWindow::changeFocus(glm::vec3 newFocus){
	focus = newFocus;
	myHistogram.ChangeFocus(focus, dataLocation);
}

void localProbeWindow::run(const char * title) {
	glfwWindow = CreateWindow(500, 200, title, nullptr, nullptr);
	if (nullptr == glfwWindow) {
		std::cout << "Failed to initialize window" << std::endl;
		closed = true;
		return;
	}
	glfwGetFramebufferSize(glfwWindow, &screenWidth, &screenHeight);
	glfwMakeContextCurrent(glfwWindow);

	glViewport(0, 0, screenWidth, screenHeight);
	
	myHistogram = Histogram(focus, numOfSubdivisions, dataLocation, xRes, yRes, zRes);

	GLuint HistogramProgram = CompileShaders("../Shaders/globalHistogram.vs", "../Shaders/globalHistogram.fs");
	glm::mat4 OrthoMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);

	while (!(glfwWindowShouldClose(glfwWindow) || shouldClose)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		myHistogram.DrawHistogram(HistogramProgram, OrthoMatrix);
		glfwSwapBuffers(glfwWindow);
	}
	glDeleteProgram(HistogramProgram);
	myHistogram.CleanUp();
	closed = true;
	return;
}

localProbeWindow::~localProbeWindow(){
	if (!closedStatus()) {
		forceClose();
	}
	if (internalThread.joinable()) {
		internalThread.join();
	}
}

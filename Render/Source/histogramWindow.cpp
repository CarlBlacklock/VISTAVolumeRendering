#include "..\Headers\histogramWindow.h"



histogramWindow::histogramWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title){
	closed = false;
	shouldClose = false;
	xRes = xResolution;
	yRes = yResolution;
	zRes = numberOfFiles;
	numOfSubdivisions = numberOfSubdivisions;
	dataLocation = volumeData;
	//std::thread myThread(this.run, title);
	internalThread = std::thread(&histogramWindow::run, this, title);
}

histogramWindow::histogramWindow(int numberOfSubdivisions, int xResolution, int yResolution, int numberOfFiles, const char* title, unsigned char* volumeData) {
	closed = false;
	shouldClose = false;
	xRes = xResolution;
	yRes = yResolution;
	zRes = numberOfFiles;
	numOfSubdivisions = numberOfSubdivisions;
	dataLocation = volumeData;
	internalThread = std::thread(&histogramWindow::run, this, title);
}

void histogramWindow::forceClose() {
	shouldClose = true;
}

bool histogramWindow::closedStatus() {
	return closed;
}
void histogramWindow::updateSubdivisions(int newSubdivisions) {
	numOfSubdivisions = newSubdivisions;
	myHistogram.CleanUp();
	myHistogram = filteredHistogram(numOfSubdivisions, xRes, yRes, zRes, histogramCalculationProgram, dataLocation);
}

void histogramWindow::framebufferCallback(GLFWwindow *histogramWindow, int width, int height) {
	glViewport(0, 0, width, height);
}
void histogramFrameBufferCallbackFunc(GLFWwindow *w, int x, int y) {
	static_cast<histogramWindow*>(glfwGetWindowUserPointer(w))->framebufferCallback(w, x, y);
}
void histogramWindow::run(const char* title) {
	glfwWindow = CreateWindow(500, 200, title, nullptr, nullptr);
	if (nullptr == glfwWindow) {
		std::cout << "Failed to initialize window" << std::endl;
		closed = true;
		return;
	}
	glfwGetFramebufferSize(glfwWindow, &screenWidth, &screenHeight);
	glfwMakeContextCurrent(glfwWindow);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, histogramFrameBufferCallbackFunc);
	glViewport(0, 0, screenWidth, screenHeight);

	histogramCalculationProgram = CompileComputeShader("../Shaders/globalHistogram.comp");

	myHistogram = filteredHistogram(numOfSubdivisions, xRes, yRes, zRes, histogramCalculationProgram, dataLocation);

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


histogramWindow::~histogramWindow(){
	if (!closedStatus()) {
		forceClose();
	}
	if (internalThread.joinable()) {
		internalThread.join();
	}
}

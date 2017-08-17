#include "localProbeWindow.h"



localProbeWindow::localProbeWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title, glm::ivec3 initialFocus){
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
	myHistogram = probeFilteredHistogram(numOfSubdivisions, xRes, yRes, zRes, HistogramCalculationProgram, dataLocation, focus);
}

void localProbeWindow::changeFocus(glm::ivec3 newFocus){
	focus = newFocus;
	focusChanged = true;
	//myHistogram.ChangeFocus(focus);
}

void probeWindowFramebufferCallbackFunc(GLFWwindow *w, int x, int y) {
	static_cast<localProbeWindow*>(glfwGetWindowUserPointer(w))->framebufferCallback(w, x, y);
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
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, probeWindowFramebufferCallbackFunc);
	glViewport(0, 0, screenWidth, screenHeight);
	
	
	HistogramCalculationProgram = CompileComputeShader("../Shaders/probeHistogram.comp");
	GLuint HistogramProgram = CompileShaders("../Shaders/globalHistogram.vs", "../Shaders/globalHistogram.fs");
	myHistogram = probeFilteredHistogram(numOfSubdivisions, xRes, yRes, zRes, HistogramCalculationProgram, dataLocation, focus);
	glm::mat4 OrthoMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);

	while (!(glfwWindowShouldClose(glfwWindow) || shouldClose)) {
		glfwPollEvents();
		if (focusChanged) {
			myHistogram.ChangeFocus(focus);
			focusChanged = false;
		}
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

#include "..\Headers\GaussianWindow.h"



GaussianWindow::GaussianWindow(const char *title){
	
	shouldClose = false;
	internalThread = std::thread(&GaussianWindow::run, this, title);
	
}


GaussianWindow::~GaussianWindow(){
	if (internalThread.joinable()) {
		internalThread.join();
	}
}
void gaussWindowFramebufferCallbackFunc(GLFWwindow *w, int x, int y) {
	static_cast<GaussianWindow*>(glfwGetWindowUserPointer(w))->framebufferCallback(w, x, y);
}

void GaussianWindow::framebufferCallback(GLFWwindow *histogramWindow, int width, int height) {
	glViewport(0, 0, width, height);
}
void GaussianWindow::run(const char * title){
	glfwWindow = CreateWindow(500, 200, title, nullptr, nullptr);
	if (nullptr == glfwWindow) {
		std::cout << "Failed to initialize window" << std::endl;
		closed = true;
		return;
	}
	glfwGetFramebufferSize(glfwWindow, &screenWidth, &screenHeight);
	glfwMakeContextCurrent(glfwWindow);
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, gaussWindowFramebufferCallbackFunc);
	glViewport(0, 0, screenWidth, screenHeight);

	GLuint gaussProgram = CompileShaders("../Shaders/Gaussian.vs", "../Shaders/Gaussian.fs");

	glm::mat4 OrthoMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
	myGaussian = Gaussian(1);
	while (!(glfwWindowShouldClose(glfwWindow) || shouldClose)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		myGaussian.DrawGaussian(gaussProgram, OrthoMatrix);
		glfwSwapBuffers(glfwWindow);
	}
	glDeleteProgram(gaussProgram);
	closed = true;
	return;
}

#include "..\Headers\renderThreads.h"
#include "..\Headers\CompileShaders.h"
#include "..\Headers\Histogram.h"
#include "..\Headers\ScreenFillingQuad.h"


//Signal used to determine if the thread should die
extern bool globalHistoClosed;
bool globalHistoShouldClose = false;

//Define externals
extern unsigned char *volumeData;
extern int xResolution;
extern int yResolution;
extern int numberOfFiles;
extern std::mutex statusMutex;
extern int status;

void histogram_framebuffer_size_callback(GLFWwindow *histogramWindow, int width, int height) {
	glViewport(0, 0, width, height);
}
void renderGlobalHistogram(int numberOfSubdivisions) {
	int i;
	GLFWwindow* globalHistogramWindow;
	globalHistogramWindow = CreateWindow(500, 200, "Global Histogram", nullptr, nullptr);

	int screenWidth, screenHeight;
	if (nullptr == globalHistogramWindow) {
		std::cout << "Failed to create GLFW window: in histo" << std::endl;

		globalHistoClosed = true;
		return;
	}

	glfwGetFramebufferSize(globalHistogramWindow, &screenWidth, &screenHeight);
	glfwMakeContextCurrent(globalHistogramWindow);
	/*
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;


		return;
	}
	*/
	glViewport(0, 0, screenWidth, screenHeight);
	
	
	glfwSetFramebufferSizeCallback(globalHistogramWindow, histogram_framebuffer_size_callback);
	Histogram globalHistogram = Histogram(numberOfSubdivisions, volumeData, xResolution, yResolution, numberOfFiles);
	
	GLuint HistogramProgram = CompileShaders("../Shaders/globalHistogram.vs", "../Shaders/globalHistogram.fs");
	//GLuint TestProgram = CompileShaders("../Shaders/test.vs", "../Shaders/global/Histogram.fs");

	glm::mat4 OrthoMatrix = glm::ortho(0.0f, 1.5f, 0.0f, 1.0f);

	statusMutex.lock();
	status++;
	statusMutex.unlock();


	while (!(glfwWindowShouldClose(globalHistogramWindow) || globalHistoShouldClose)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);
		GLenum errorType;
		while ((errorType = glGetError()) != GL_NO_ERROR) {

		}
		globalHistogram.DrawHistogram(HistogramProgram, OrthoMatrix);
		//test.DrawScreenFillingQuad(HistogramProgram, OrthoMatrix, OrthoMatrix, (GLuint)1);
		glfwSwapBuffers(globalHistogramWindow);
	}
	glDeleteProgram(HistogramProgram);
	globalHistogram.CleanUp();
	
	globalHistoClosed = true;
	return;
}
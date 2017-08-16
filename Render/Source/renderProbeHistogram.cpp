#include "..\Headers\renderThreads.h"
#include "..\Headers\Utilities.h"
#include "..\Headers\CompileShaders.h"
#include "..\Headers\Histogram.h"

//Signal used to determine if the thread should die
extern bool probeHistoClosed;
bool probeHistoShouldClose = false;


//Define externals
extern unsigned char *volumeData;
extern int xResolution;
extern int yResolution;
extern int numberOfFiles;
extern float cursorVolumePosition[3];

//Define probeFocus
glm::vec3 probeFocus;
std::mutex probeFocusLock;
bool probeFocusChanged = false;

void probe_histogram_framebuffer_size_callback(GLFWwindow *histogramWindow, int width, int height) {
	glViewport(0, 0, width, height);
}

void renderProbeHistogram(int numberOfSubdivisions) {
	int i;
	GLFWwindow* localHistogramWindow;
	localHistogramWindow = CreateWindow(500, 200, "Probe Histogram", nullptr, nullptr);

	int screenWidth, screenHeight;
	if (nullptr == localHistogramWindow) {
		std::cout << "Failed to create GLFW window: in histo" << std::endl;

		probeHistoClosed = true;
		return;
	}

	glfwGetFramebufferSize(localHistogramWindow, &screenWidth, &screenHeight);
	glfwMakeContextCurrent(localHistogramWindow);
	glViewport(0, 0, screenWidth, screenHeight);


	glfwSetFramebufferSizeCallback(localHistogramWindow, probe_histogram_framebuffer_size_callback);

	GLuint sobelGaussFilterProgram = CompileComputeShader("../Shaders/sobelGaussFilter.comp");
	GLuint sobelGaussFilterID;
	sobelGaussFilter(sobelGaussFilterProgram, xResolution, yResolution, numberOfFiles, &sobelGaussFilterID);


	GLuint HistogramProgram = CompileComputeShader("../Shaders/localHistogram.comp");
	GLuint HistogramDrawProgram = CompileShaders("../Shaders/globalHistogram.vs", "../Shaders/globalHistogram.fs");

	probeFocusLock.lock();
	probeFocus = glm::vec3(0.5f, 0.5f, 0.5f);
	Histogram local = Histogram(probeFocus, numberOfSubdivisions, volumeData, xResolution, yResolution, numberOfFiles);
	probeFocusLock.unlock();

	glm::mat4 OrthoMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);

	while (!(glfwWindowShouldClose(localHistogramWindow) || probeHistoShouldClose)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		if (probeFocusChanged) {
			probeFocusLock.lock();
			local.ChangeFocus(probeFocus, volumeData);
			probeFocusLock.unlock();
			probeFocusChanged = false;
		}
		glClear(GL_COLOR_BUFFER_BIT);
		GLenum errorType;
		while ((errorType = glGetError()) != GL_NO_ERROR) {

		}
		local.DrawHistogram(HistogramDrawProgram, OrthoMatrix);
		//test.DrawScreenFillingQuad(HistogramProgram, OrthoMatrix, OrthoMatrix, (GLuint)1);
		glfwSwapBuffers(localHistogramWindow);
	}
	glDeleteProgram(HistogramProgram);
	glDeleteProgram(sobelGaussFilterProgram);
	local.CleanUp();
	glDeleteProgram(HistogramDrawProgram);
	probeHistoClosed = true;
	return;
}
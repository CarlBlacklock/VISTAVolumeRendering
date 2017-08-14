#include "..\Headers\renderThreads.h"

#include "..\Headers\CompileShaders.h"
#include "..\Headers\Histogram.h"

//Signal used to determine if the thread should die
extern bool localHistoClosed;
bool localHistoShouldClose = false;


//Define externals
extern unsigned char *volumeData;
extern int xResolution;
extern int yResolution;
extern int numberOfFiles;
extern float cursorVolumePosition[3];
extern int numberOfSubdivisions;

//Define focus
glm::vec3 focus;
std::mutex focusLock;
bool focusChanged = false;

void local_histogram_framebuffer_size_callback(GLFWwindow *histogramWindow, int width, int height) {
	glViewport(0, 0, width, height);
}

void renderLocalHistogram() {
	int i;
	GLFWwindow* localHistogramWindow;
	localHistogramWindow = CreateWindow(500, 200, "Local Histogram", nullptr, nullptr);

	int screenWidth, screenHeight;
	if (nullptr == localHistogramWindow) {
		std::cout << "Failed to create GLFW window: in histo" << std::endl;

		localHistoClosed = true;
		return;
	}

	glfwGetFramebufferSize(localHistogramWindow, &screenWidth, &screenHeight);
	glfwMakeContextCurrent(localHistogramWindow);
	glViewport(0, 0, screenWidth, screenHeight);


	glfwSetFramebufferSizeCallback(localHistogramWindow, local_histogram_framebuffer_size_callback);

	GLuint HistogramProgram = CompileComputeShader("../Shaders/localHistogram.comp");
	GLuint HistogramDrawProgram = CompileShaders("../Shaders/globalHistogram.vs", "../Shaders/globalHistogram.fs");

	focusLock.lock();
	focus = glm::vec3(0.5f, 0.5f, 0.5f);
	Histogram local = Histogram(focus, numberOfSubdivisions, volumeData, xResolution, yResolution, numberOfFiles, HistogramProgram);
	focusLock.unlock();

	glm::mat4 OrthoMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);

	while (!(glfwWindowShouldClose(localHistogramWindow) || localHistoShouldClose)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		if (focusChanged) {
			focusLock.lock();
			local.ChangeFocus(focus, HistogramProgram, volumeData);
			focusLock.unlock();
			focusChanged = false;
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
	local.CleanUp();
	glDeleteProgram(HistogramDrawProgram);
	localHistoClosed = true;
	return;
}
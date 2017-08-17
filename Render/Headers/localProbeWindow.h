#pragma once
#include "..\Headers\Render.h"
#include "..\Headers\probeFilteredHistogram.h"
#include <thread>
#include "..\Headers\CompileShaders.h"
class localProbeWindow
{
public:
	localProbeWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title, glm::ivec3 initialFocus);
	bool closedStatus();
	void forceClose();
	void updateSubdivisions(int newSubdivisions);
	void changeFocus(glm::ivec3 newFocus);
	~localProbeWindow();
	void framebufferCallback(GLFWwindow *histogramWindow, int width, int height);
private:
	glm::ivec3 focus;
	probeFilteredHistogram myHistogram;
	bool closed;
	bool shouldClose;
	GLFWwindow* glfwWindow;
	int screenWidth, screenHeight;
	bool focusChanged;
	int xRes, yRes, zRes;
	int numOfSubdivisions;
	unsigned char *dataLocation;
	std::thread internalThread;
	void run(const char * title);
	GLuint HistogramCalculationProgram;
};


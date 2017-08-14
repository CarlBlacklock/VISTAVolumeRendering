#pragma once
#include "..\Headers\Render.h"
#include "..\Headers\Histogram.h"
#include <thread>
#include "..\Headers\CompileShaders.h"
class localProbeWindow
{
public:
	localProbeWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title, glm::vec3 initialFocus);
	bool closedStatus();
	void forceClose();
	void updateSubdivisions(int newSubdivisions);
	void changeFocus(glm::vec3 newFocus);
	~localProbeWindow();

private:
	glm::vec3 focus;
	Histogram myHistogram;
	bool closed;
	bool shouldClose;
	GLFWwindow* glfwWindow;
	int screenWidth, screenHeight;
	void framebufferCallback(GLFWwindow *histogramWindow, int width, int height);
	int xRes, yRes, zRes;
	int numOfSubdivisions;
	unsigned char *dataLocation;
	std::thread internalThread;
	void run(const char * title);
};


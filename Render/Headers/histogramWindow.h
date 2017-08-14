#pragma once
#include "..\Headers\Render.h"
#include "..\Headers\Histogram.h"
#include <thread>
#include "..\Headers\CompileShaders.h"
class histogramWindow{
public:
	histogramWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title);
	bool closedStatus();
	void forceClose();
	void updateSubdivisions(int newSubdivisions);
	~histogramWindow();

private:
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


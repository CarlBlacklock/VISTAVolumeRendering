#pragma once
#include "..\Headers\Render.h"
#include "..\Headers\filteredHistogram.h"
#include <thread>
#include "..\Headers\CompileShaders.h"
class histogramWindow{
public:
	histogramWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title);
	histogramWindow(int numberOfSubdivisions, int xResolution, int yResolution, int numberOfFiles,const char*title, unsigned char* volumeData);
	bool closedStatus();
	void forceClose();
	void updateSubdivisions(int newSubdivisions);
	~histogramWindow();

private:
	filteredHistogram myHistogram;
	GLuint volume;
	GLuint histogramCalculationProgram;
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


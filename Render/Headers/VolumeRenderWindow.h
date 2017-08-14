#pragma once
#include "..\Headers\Render.h"
#include "..\Headers\Histogram.h"
#include <thread>
#include "..\Headers\CompileShaders.h"
#include "..\Headers\histogramWindow.h"
#include "..\Headers\localProbeWindow.h"
#include "..\Headers\Camera.h"
#include "..\Headers\Utilities.h"
#include "..\Headers\ScreenFillingQuad.h"
#include "..\Headers\DataCube.h"
class VolumeRenderWindow
{
public:
	VolumeRenderWindow(int numberOfSubdivisions, unsigned char* volumeData, int xResolution, int yResolution, int numberOfFiles, const char* title);
	~VolumeRenderWindow();

private:
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
	localProbeWindow *attachedProbeWindow;
	histogramWindow *attachedHistogramWindow;
	DataCube* myCube;
	Camera* myCamera;
};


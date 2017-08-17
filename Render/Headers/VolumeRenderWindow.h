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
	void framebufferCallback(GLFWwindow *histogramWindow, int width, int height);
	void VolumeRenderCursorCallBack(GLFWwindow * window, double xpos, double ypos);
	void VolumeRenderMouseButtonCallBack(GLFWwindow * window, int button, int action, int mods);
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	bool closedStatus();
	bool isGLEWINIT();
	void forceClose();
	void attachProbeWindow(localProbeWindow* probeWindow);
	void attachLocalHistogramWindow(localProbeWindow* localHistogramWindow);
	void attachHistogramWindow(histogramWindow* histogramWindow);

private:
	bool closed;
	bool shouldClose;
	GLFWwindow* glfwWindow;
	int screenWidth, screenHeight;
	
	int xRes, yRes, zRes;
	int numOfSubdivisions;
	unsigned char *dataLocation;
	std::thread internalThread;
	void run(const char * title);
	
	localProbeWindow *attachedProbeWindow;
	localProbeWindow *attachedLocalHistogramWindow;
	histogramWindow *attachedHistogramWindow;
	DataCube* myCube;
	Camera* myCamera;
	GLuint volumeID, sobelGaussFilterID, colorTex, depthTex, posTex, fboHandle;
	bool GLEWINIT;
	float *extents;
	int renderMode, filterMode;
	float cursorVolumePosition[3] = { 0.0f, 0.0f, 0.0f };
};


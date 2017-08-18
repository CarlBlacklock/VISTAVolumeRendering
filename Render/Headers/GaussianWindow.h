#pragma once
#include "../Headers/Render.h"
#include "../Headers/Gaussian.h"
class GaussianWindow
{
public:
	bool closedStatus();
	void forceClose();
	float getA();
	float getB();
	float getC();
	void addA(float newA);
	void addB(float newB);
	void addC(float newC);
	GaussianWindow(const char *title);
	~GaussianWindow();
	void framebufferCallback(GLFWwindow *histogramWindow, int width, int height);

private:
	bool closed;
	bool shouldClose;
	GLFWwindow* glfwWindow;
	int screenWidth, screenHeight;
	Gaussian myGaussian;
	std::thread internalThread;
	void run(const char * title);
};


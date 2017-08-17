#include "..\Headers\Render.h"
#include "..\Headers\LoadDicom.h"
#include "..\Headers\Utilities.h"
#include "..\Headers\histogramWindow.h"
#include "..\Headers\localProbeWindow.h"
#include "..\Headers\VolumeRenderWindow.h"
#include "..\Headers\GaussianWindow.h"
#include <thread>

int xResolution = 256;
int yResolution = xResolution;
int numberOfFiles = 225;
int numberOfSubdivisions = 20;
unsigned char *volumeData;

int main() {
	int i; 
	if (!glfwInit()) {
		std::cout << "Failed to initialize glfw" << std::endl;
	}
	
	//Read the volume data
	//unsigned char *volumeData = loadRAW("../Resources/Foot.raw", &zDistance);
	std::cout << "Loading Volume Data" << std::endl;
	volumeData = LoadDicom("../Resources/exported/IMG-0001-", xResolution, yResolution, numberOfFiles);
	//float *boneMap = readColorMap("../Resources/boneColorMap.m");
	if (nullptr == volumeData) {
		std::cout << "Error loading volume data. Press any key to exit" << std::endl;
		std::cin >> i;
		return EXIT_FAILURE;
	}
	std::cout << "Volume Data Loaded" << std::endl;
	
	VolumeRenderWindow renderWindow(numberOfSubdivisions, volumeData, xResolution, yResolution, numberOfFiles, "VISTA Volume Rendering");
	while (!(renderWindow.isGLEWINIT() || renderWindow.closedStatus())) {

	}
	if (!(renderWindow.isGLEWINIT())) {
		//Something went wrong with initializing GLEW
		//Abort
		std::cout << "Error initializing GLEW" << std::endl;
		std::cin >> i;
		return EXIT_FAILURE;
	}
	
	//std::thread globalHistoRender(renderGlobalHistogram);
	histogramWindow globalHistogramWindow(numberOfSubdivisions, xResolution, yResolution, numberOfFiles, "Global Histogram", volumeData);
	localProbeWindow probeWindow(numberOfSubdivisions, volumeData, xResolution, yResolution, numberOfFiles, "Probe Histogram", glm::vec3(0.0f, 0.0f, 0.0f));
	localProbeWindow localHistogramWindow(numberOfSubdivisions, volumeData, xResolution, yResolution, numberOfFiles, "Local Histogram", glm::ivec3(0, 0, 0));
	GaussianWindow gaussWindow("Gaussian");
	//std::thread localHistoRender(renderLocalHistogram);
	//std::thread probeHistorRender(renderProbeHistogram, numberOfSubdivisions);
	
	renderWindow.attachHistogramWindow(&globalHistogramWindow);
	renderWindow.attachProbeWindow(&probeWindow);
	renderWindow.attachLocalHistogramWindow(&localHistogramWindow);
	
	
	while (!(renderWindow.closedStatus() || probeWindow.closedStatus() || localHistogramWindow.closedStatus() || globalHistogramWindow.closedStatus())){

	}
	if (renderWindow.closedStatus()) {
		//One of the threads terminated
		//Send the signal to end
		std::cout << "histogram Closed" << std::endl;
		probeWindow.forceClose();
		localHistogramWindow.forceClose();
		globalHistogramWindow.forceClose();
	}
	
	else if (probeWindow.closedStatus()) {
		renderWindow.forceClose();
		localHistogramWindow.forceClose();
		globalHistogramWindow.forceClose();
	}
	
	else if (localHistogramWindow.closedStatus()) {
		renderWindow.forceClose();
		globalHistogramWindow.forceClose();
		probeWindow.forceClose();
	}
	else if (globalHistogramWindow.closedStatus()) {
		renderWindow.forceClose();
		localHistogramWindow.forceClose();
		probeWindow.forceClose();
	}
	std::cout << "About to close" << std::endl;
	//std::cin >> i;
	//marchCubes(0.2f, &numberOfVerticesCalculated, &placeholderVAO, xResolution, yResolution, numberOfFiles);
	
	globalHistogramWindow.~histogramWindow();
	probeWindow.~localProbeWindow();
	localHistogramWindow.~localProbeWindow();
	renderWindow.~VolumeRenderWindow();
	gaussWindow.~GaussianWindow();
	glfwTerminate();
	free(volumeData);
	std::cout << "Volume Data released" << std::endl;
	return EXIT_SUCCESS;
}
#include "..\Headers\Render.h"
#include "..\Headers\LoadDicom.h"
#include "..\Headers\renderThreads.h"
#include "..\Headers\Utilities.h"
#include "..\Headers\histogramWindow.h"
#include "..\Headers\localProbeWindow.h"
#include <thread>

int xResolution = 256;
int yResolution = xResolution;
int numberOfFiles = 225;
int numberOfSubdivisions = 20;
unsigned char *volumeData;

bool globalHistoClosed = false;
bool localHistoClosed = false;
bool probeHistoClosed = false;

extern bool globalHistoShouldClose;
bool volumeRenderClosed = false;
extern bool volumeRenderShouldClose;
extern bool GLEWINIT;
extern bool localHistoShouldClose;
extern bool probeHistoShouldClose;



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
	
	std::thread volumeRender(renderVolume);
	while (!GLEWINIT) {

	}
	
	//std::thread globalHistoRender(renderGlobalHistogram);
	histogramWindow globalHistogramWindow(numberOfSubdivisions, volumeData, xResolution, yResolution, numberOfFiles, "Global Histogram");
	localProbeWindow probeWindow(numberOfSubdivisions, volumeData, xResolution, yResolution, numberOfFiles, "Probe Histogram", glm::vec3(0.0f, 0.0f, 0.0f));
	//std::thread localHistoRender(renderLocalHistogram);
	std::thread probeHistorRender(renderProbeHistogram, numberOfSubdivisions);
	
	if (globalHistoClosed) {
		//One of the threads terminated
		//Send the signal to end
		volumeRenderShouldClose = true;
		localHistoShouldClose = true;

	}
	else if (volumeRenderClosed) {
		globalHistoShouldClose = true;
		localHistoShouldClose = true;
	}
	else if (localHistoClosed) {
		volumeRenderShouldClose = true;
		globalHistoShouldClose = true;
	}
	
	
	while (!(globalHistoClosed || volumeRenderClosed)) {
		//Wait on the threads
		
	}
	if (globalHistoClosed) {
		//One of the threads terminated
		//Send the signal to end
		std::cout << "histogram Closed" << std::endl;
		volumeRenderShouldClose = true;
		localHistoShouldClose = true;
		probeHistoShouldClose = true;
	}
	else if (volumeRenderClosed) {
		globalHistoShouldClose = true;
		localHistoShouldClose = true;
		probeHistoShouldClose = true;
		globalHistogramWindow.forceClose();
		probeWindow.forceClose();
	}
	std::cout << "About to close" << std::endl;
	//std::cin >> i;
	//marchCubes(0.2f, &numberOfVerticesCalculated, &placeholderVAO, xResolution, yResolution, numberOfFiles);
	
	globalHistogramWindow.~histogramWindow();
	probeWindow.~localProbeWindow();
	//globalHistoRender.join();
	volumeRender.join();
	//localHistoRender.join();
	probeHistorRender.join();
	glfwTerminate();
	free(volumeData);
	std::cout << "Volume Data released" << std::endl;
	return EXIT_SUCCESS;
}
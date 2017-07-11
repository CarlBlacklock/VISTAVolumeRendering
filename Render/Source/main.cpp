#include "..\Headers\Render.h"
#include "..\Headers\LoadDicom.h"
#include "..\Headers\renderThreads.h"

#include <thread>

int xResolution = 256;
int yResolution = xResolution;
int numberOfFiles = 225;
unsigned char *volumeData;
std::mutex statusMutex;
int status = 0;

bool globalHistoClosed = false;

extern bool globalHistoShouldClose;
bool volumeRenderClosed = false;
extern bool volumeRenderShouldClose;
extern bool GLEWINIT;



int main() {
	int i; 
	if (!glfwInit()) {
		std::cout << "Failed to initialize glfw" << std::endl;
	}
	
	//Read the volume data
	//unsigned char *volumeData = loadRAW("../Resources/Foot.raw", &zDistance);
	std::cout << "Loading Volume Data" << std::endl;
	volumeData = LoadDicom("../Resources/exported/IMG-0001-", xResolution, yResolution, numberOfFiles);
	if (nullptr == volumeData) {
		std::cout << "Error loading volume data. Press any key to exit" << std::endl;
		std::cin >> i;
		return EXIT_FAILURE;
	}
	std::cout << "Volume Data Loaded" << std::endl;
	/*
	std::thread volumeRender(renderVolume);
	while (!GLEWINIT) {

	}
	*/
	std::thread globalHistoRender(renderGlobalHistogram);
	while (status < 1 && !(globalHistoClosed || volumeRenderClosed)) {
		//Wait until all threads are finished using the volume data
	}
	std::cout << "All threads done with volume data" << std::endl;
	if (globalHistoClosed) {
		//One of the threads terminated
		//Send the signal to end
		volumeRenderShouldClose = true;

	}
	else if (volumeRenderClosed) {
		globalHistoShouldClose = true;
	}
	
	free(volumeData);
	std::cout << "Volume Data released" << std::endl;
	while (!(globalHistoClosed || volumeRenderClosed)) {
		//Wait on the threads
		
	}
	if (globalHistoClosed) {
		//One of the threads terminated
		//Send the signal to end
		std::cout << "histogram Closed" << std::endl;
		volumeRenderShouldClose = true;
	}
	else if (volumeRenderClosed) {
		globalHistoShouldClose = true;
	}
	std::cout << "About to close" << std::endl;
	//std::cin >> i;
	//marchCubes(0.2f, &numberOfVerticesCalculated, &placeholderVAO, xResolution, yResolution, numberOfFiles);
	glfwTerminate();

	globalHistoRender.join();
	return EXIT_SUCCESS;
}
#include "..\Headers\RAWLoader.h"
#include <string>
#include <fstream>
#include <sstream>

unsigned char * loadRAW(const char * pathToRAW, int *numberOfReads){
	size_t elementsRead;
	FILE *rawFile = fopen(pathToRAW, "rb");
	//Please note for anyone else reading this make sure you open all files in binary mode otherwise this won't work
	if (rawFile == NULL) {
		//Couldn't open the file return null pointer
		return nullptr;
	}
	
	unsigned char *rawBytes;
	
	//Currently handles only images that are 256x256
	//Need to add functionality for arbitrary sized images
	rawBytes = (unsigned char *)malloc(256* 256 * 256 * sizeof(unsigned char));
	*numberOfReads = 1;
	//Allocate enough memory for one 256x256 image
	elementsRead = fread(rawBytes, sizeof(unsigned char), 256 * 256*256, rawFile);
	std::cout << "Elements Read: " << elementsRead << std::endl;
	int timesThroughLoop = 0;
	//keep reading file until EOF is encountered
	while(fgetc(rawFile) != EOF){
		*numberOfReads = *numberOfReads + 1;
		//allocate memory for new image
		rawBytes = (unsigned char *)realloc(rawBytes, (256 * 256 * (*numberOfReads) * sizeof(unsigned char)));
		if (rawBytes == NULL) {
			//memory reallocation has failed -- ABORT MISSION
			return nullptr;
		}
		fseek(rawFile, (long)-1, SEEK_CUR);
		fread(rawBytes, sizeof(unsigned char), 256 * 256, rawFile);
		timesThroughLoop++;
	}
	std::cout << "Loop iterations: " << timesThroughLoop << std::endl;
	std::cout << "Volume data depth: " << *numberOfReads << std::endl;
	return rawBytes;
	
}

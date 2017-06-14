#include "../Headers/LoadDicom.h"
#include <string>

unsigned char *LoadDicom(const char *basePath, int xResolution, int yResolution, int numberOfFiles) {
	std::string basicPath(basePath);
	unsigned char *data;
	data = (unsigned char *)malloc(xResolution * yResolution * numberOfFiles * sizeof(unsigned char));
	if (data == NULL) {
		return nullptr;
	}
	else {
		int numberOfZeroesToAdd, powerOfTen;
		int imageWidth, imageHeight;
		int offset = 0;
		for (int i = 1; i <= numberOfFiles; i++) {
			int currentNumber = i;
			numberOfZeroesToAdd = 4;
			while (currentNumber / 10 != 0) {
				currentNumber = currentNumber / 10;
				numberOfZeroesToAdd--;
			}
			std::string cppCurrentFile(basicPath);
			for (int j = 0; j < numberOfZeroesToAdd; j++) {
				cppCurrentFile.append("0");
			}
			cppCurrentFile.append(std::to_string(i));
			cppCurrentFile.append(".bmp");
			const char *currentFilePath = cppCurrentFile.c_str();
			unsigned char *currentImageData = SOIL_load_image(currentFilePath, &imageWidth, &imageHeight, 0, SOIL_LOAD_L);
			offset = xResolution * yResolution * (i-1);
			for (int k = 0; k < xResolution * yResolution; k++) {
				data[offset + k] = currentImageData[k];
			}
			SOIL_free_image_data(currentImageData);
		}
		return data;
	}
}
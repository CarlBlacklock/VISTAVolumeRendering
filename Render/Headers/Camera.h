#pragma once
#include "..\Headers\Render.h"
class Camera {

public:
	glm::vec3 LookFrom;
	glm::vec3 LookUp;
	glm::mat4 ViewMatrix;
	glm::vec3 ViewDir;
	Camera();
	void moveCamera(float deltaTheta, float deltaPhi);
	void ResetCamera();
private:
	float Theta;
	float Phi;
};
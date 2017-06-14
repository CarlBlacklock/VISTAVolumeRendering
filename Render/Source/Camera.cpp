#include "..\Headers\Camera.h"

Camera::Camera() {
	Phi = 0.0f;
	Theta = M_PI/2.0f;
	LookFrom = glm::vec3((float)(sin(Theta)*cos(Phi)), (float)cos(Theta), (float)(sin(Theta)*sin(Phi)));
	LookUp = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewDir = glm::vec3(0.0f, 0.0f, 0.0f) - LookFrom;
	ViewMatrix = glm::lookAt(LookFrom, glm::vec3(0.0f, 0.0f, 0.0f), LookUp);

}

void Camera::moveCamera(float deltaTheta, float deltaPhi){
	if (Phi >= 2.0f * M_PI && deltaPhi > 0.0f){
		//|| Phi < 0.0f || Theta > M_PI || Theta < 0.0f) {
		deltaPhi = -Phi;
	}
	if (Phi <= 0.0f && deltaPhi < 0.0f) {
		deltaPhi = -Phi;
	}
	if (Theta >= M_PI && deltaTheta > 0.0f) {
		deltaTheta = -Theta;
	}
	if (Theta <= 0.0f && deltaTheta < 0.0f) {
		deltaTheta = Theta;
	}
	
	Theta += deltaTheta;
	Phi += deltaPhi;
	
	LookFrom = glm::vec3((float)(sin(Theta)*cos(Phi)), (float)cos(Theta), (float)(sin(Theta)*sin(Phi)));
	glm::vec3 Right = glm::cross(LookFrom, LookUp);
	LookUp = glm::cross(Right, LookFrom);
	ViewDir = glm::vec3(0.0f, 0.0f, 0.0f) - LookFrom;
	ViewMatrix = glm::lookAt(LookFrom, glm::vec3(0.0f, 0.0f, 0.0f), LookUp);
}

void Camera::ResetCamera() {
	Phi = 0.0f;
	Theta = M_PI / 2.0f;
	LookFrom = glm::vec3((float)(sin(Theta)*cos(Phi)), (float)cos(Theta), (float)(sin(Theta)*sin(Phi)));
	LookUp = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewDir = glm::vec3(0.0f, 0.0f, 0.0f) - LookFrom;
	ViewMatrix = glm::lookAt(LookFrom, glm::vec3(0.0f, 0.0f, 0.0f), LookUp);
}

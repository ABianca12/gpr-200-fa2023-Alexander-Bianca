#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

namespace MyLibrary
{
	struct Camera
	{
		ew::Vec3 position = (0, 0, 5);
		ew::Vec3 target = (0, 0, 0);
		float fov = 70;
		float aspectRatio = 4/3;
		float nearPlane = 0.1;
		float farPlane = 100.0;
		bool orthographic = true;
		float orthoSize = 6;
		ew::Mat4 ViewMatrix();
		ew::Mat4 ProjectionMatrix();

		const ew::Vec3 INITAL_POS = position;
		const ew::Vec3 INITAL_TARGET = target;
		const float INITAL_FOV = fov;
		const float INITAL_NEAR_PLANE = nearPlane;
		const float INITAL_FAR_PLANE = farPlane;
		const float INITAL_ORTHO_SIZE = orthoSize;

		void ResetCam()
		{
			position = INITAL_POS;
			target = INITAL_TARGET;
			fov = INITAL_FOV;
			nearPlane = INITAL_NEAR_PLANE;
			farPlane = INITAL_FAR_PLANE;
			orthoSize = INITAL_ORTHO_SIZE;
		}
	};

	struct CameraControls
	{
		double prevMouseX, prevMouseY;
		float yaw = 0, pitch = 0;
		float mouseSensitivity = 5.0;
		bool firstMouse = true;
		float moveSpeed = 5.0f;
		bool invertY = false;

		const float INITAL_YAW = yaw;
		const float INITAL_PITCH = pitch;

		void ResetCameraControls()
		{
			yaw = INITAL_YAW;
			pitch = INITAL_PITCH;
		}
	};
}
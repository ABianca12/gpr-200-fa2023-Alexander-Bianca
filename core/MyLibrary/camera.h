#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

namespace MyLibrary
{
	struct Camera
	{
		ew::Vec3 position = (0, 0, 5);
		ew::Vec3 target = (0, 0, 0);
		float fov = 90;
		float aspectRatio = 4/3;
		float nearPlane = 0.1;
		float farPlane = 100.0;
		bool orthographic = true;
		float orthoSize = 6;
		ew::Mat4 ViewMatrix();
		ew::Mat4 ProjectionMatrix();
	};

	struct CameraControls
	{
		double prevMouseX, prevMouseY;
		float yaw = 0, pitch = 0;
		float mouseSensitivity = 0.1;
		bool firstMouse = true;
		float moveSpeed = 5.0f;
	};
}
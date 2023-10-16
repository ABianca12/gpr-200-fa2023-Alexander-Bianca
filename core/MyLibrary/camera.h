#pragma once

namespace MyLibrary
{
	struct Camera
	{
		ew::Vec3 position;
		ew::Vec3 target;
		float fov;
		float aspectRatio;
		float nearPlane;
		bool orthographic;
		float orthoSize;
		ew::Mat4 ViewMatrix();
		ew::Mat4 ProjectionMatrix();
	};

	struct CameraControls
	{
		double pervMouseX, pervMouseY;
		float yaw = 0, pitch = 0;
		float mouseSensitivity = 0.1;
		bool firstMouse = true;
		float moveSpeed = 5.0f;
	};
}
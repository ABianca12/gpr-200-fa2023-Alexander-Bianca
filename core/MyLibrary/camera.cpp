#include "camera.h"
#include "camera.h"
#include "transformations.h"

namespace MyLibrary
{
	ew::Mat4 Camera::ViewMatrix(){
		return MyLibrary::LookAt(position, target);
	}

	ew::Mat4 Camera::ProjectionMatrix()
	{
		if (orthographic)
		{
			return MyLibrary::Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
		}
		else
			return MyLibrary::Perspective(fov * ew::DEG2RAD, aspectRatio, nearPlane, farPlane);
	}
}
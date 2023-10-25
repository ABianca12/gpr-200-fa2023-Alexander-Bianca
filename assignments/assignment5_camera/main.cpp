#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <MyLibrary/camera.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void moveCamera(GLFWwindow* window, MyLibrary::Camera* camera, MyLibrary::CameraControls* controls, float deltaTime);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

int resolution[2] = { SCREEN_WIDTH, SCREEN_HEIGHT };

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

MyLibrary::Camera cam;
MyLibrary::CameraControls cameraControl;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	cam.position = { 0, 0, 5 };

	MyLibrary::Camera camera;
	MyLibrary::CameraControls cameraControls;

	float prevTime = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		moveCamera(window, &cam, &cameraControls, deltaTime);
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cam.aspectRatio = (float)resolution[0] / resolution[1];

		//Set uniforms
		shader.use();
		shader.setMat4("_View", cam.ViewMatrix());
		shader.setMat4("_Projection", cam.ProjectionMatrix());

		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}

			if (ImGui::CollapsingHeader("Camera"))
			{
				ImGui::DragFloat3("Camera Position", &cam.position.x, 0.01f);
				ImGui::DragFloat3("Camera Target", &cam.target.x, 0.01f);
				ImGui::Checkbox("Orthographic", &cam.orthographic);
				if (cam.orthographic)
				{
					ImGui::DragFloat("Orhtographic Frustum Height", &cam.orthoSize);
				}
				else
				{
					ImGui::DragFloat("FOV", &cam.fov, 0.1f);
					ImGui::DragFloat("Near Plane", &cam.nearPlane, 0.1f);
					ImGui::DragFloat("Far Plane", &cam.farPlane, 0.1f);
				}
					
			}

			if (ImGui::CollapsingHeader("Camera Controller"))
			{
				ImGui::Text("Yaw: %.0f", cameraControls.yaw);
				ImGui::Text("Pitch : %.0f", cameraControls.pitch);
				ImGui::DragFloat("Move Speed", &cameraControls.moveSpeed, 0.1f);
				ImGui::DragFloat("Sensitivity", &cameraControls.mouseSensitivity);
				ImGui::Checkbox("Invert Y", &cameraControls.invertY);
			}

			if (ImGui::Button("Reset"))
			{
				cam.ResetCam();
				cameraControls.ResetCameraControls();
			}

			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void moveCamera(GLFWwindow* window, MyLibrary::Camera* camera, MyLibrary::CameraControls* controls, float deltaTime)
{
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		if (controls->firstMouse)
		{
			controls->firstMouse = false;
			controls->prevMouseX = mouseX;
			controls->prevMouseY = mouseY;
		}

		double mouseXDelta = mouseX - controls->prevMouseX;
		double mouseYDelta = mouseY - controls->prevMouseY;

		if (!cameraControl.invertY)
		{
			mouseYDelta *= -1;
		}

		controls->yaw += mouseXDelta * controls->mouseSensitivity * deltaTime;
		controls->pitch += mouseYDelta * controls->mouseSensitivity * deltaTime;

		if (controls->pitch > 89)
		{
			controls->pitch = 89;
		}
		else if (controls->pitch < -89)
		{
			controls->pitch = -89;
		}
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	double yawRad = controls->yaw * ew::DEG2RAD;
	double pitchRad = controls->pitch * ew::DEG2RAD;
	ew::Vec3 forward = ew::Vec3(sin(yawRad) * cos(pitchRad), sin(pitchRad), -cos(yawRad) * cos(pitchRad));
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, ew::Vec3(0, 1, 0)));
	ew::Vec3 up = ew::Normalize(ew::Cross(right, forward));

	if (glfwGetKey(window, GLFW_KEY_W))
	{
		camera->position += forward * controls->moveSpeed * deltaTime;
	}
		
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		camera->position += -1 * forward * controls->moveSpeed * deltaTime;
	}
		
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		camera->position += -1 * right * controls->moveSpeed * deltaTime;
	}
		
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		camera->position += right * controls->moveSpeed * deltaTime;
	}
		
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		camera->position += -1 * up * controls->moveSpeed * deltaTime;
	}
		
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		camera->position += up * controls->moveSpeed * deltaTime;
	}
	camera->target = camera->position + forward;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	resolution[0] = width;
	resolution[1] = height;
}


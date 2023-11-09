#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <MyLibrary/procGen.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;

struct AppSettings {
	const char* shadingModeNames[6] = { "Solid Color","Normals","UVs","Texture","Lit","Texture Lit"};
	int shadingModeIndex;

	ew::Vec3 bgColor = ew::Vec3(0.1f);
	ew::Vec3 shapeColor = ew::Vec3(1.0f);
	bool wireframe = true;
	bool drawAsPoints = false;
	bool backFaceCulling = true;

	//Euler angles (degrees)
	ew::Vec3 lightRotation = ew::Vec3(0, 0, 0);
}appSettings;

ew::Camera camera;
ew::CameraController cameraController;

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
	glPointSize(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	MyLibrary::Cube cube;
	MyLibrary::Plane plane;
	MyLibrary::Cylinder cylinder;
	MyLibrary::Sphere sphere;

	cube.transform.position = ew::Vec3(-2, 0, 0);
	plane.transform.position = ew::Vec3(2, 0, 0);
	cylinder.transform.position = ew::Vec3(4, 0, 0);
	sphere.transform.position = ew::Vec3(-4, 1, 0);

	resetCamera(camera, cameraController);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		cameraController.Move(window, &camera, deltaTime);

		//Render
		glClearColor(appSettings.bgColor.x, appSettings.bgColor.y, appSettings.bgColor.z,1.0f);

		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setInt("_Mode", appSettings.shadingModeIndex);
		shader.setVec3("_Color", appSettings.shapeColor);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Euler angels to forward vector
		ew::Vec3 lightRot = appSettings.lightRotation * ew::DEG2RAD;
		ew::Vec3 lightF = ew::Vec3(sinf(lightRot.y) * cosf(lightRot.x), sinf(lightRot.x), -cosf(lightRot.y) * cosf(lightRot.x));
		shader.setVec3("_LightDir", lightF);

		shader.setMat4("_Model", cube.transform.getModelMatrix());
		cube.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);

		shader.setMat4("_Model", plane.transform.getModelMatrix());
		plane.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);
		
		shader.setMat4("_Model", cylinder.transform.getModelMatrix());
		cylinder.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);
		
		shader.setMat4("_Model", sphere.transform.getModelMatrix());
		sphere.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			if (ImGui::CollapsingHeader("Rendering Settings"))
			{
				ImGui::ColorEdit3("BG color", &appSettings.bgColor.x);
				ImGui::ColorEdit3("Shape color", &appSettings.shapeColor.x);
				ImGui::Combo("Shading mode", &appSettings.shadingModeIndex, appSettings.shadingModeNames, IM_ARRAYSIZE(appSettings.shadingModeNames));
				if (appSettings.shadingModeIndex > 3) {
					ImGui::DragFloat3("Light Rotation", &appSettings.lightRotation.x, 1.0f);
				}
				ImGui::Checkbox("Draw as points", &appSettings.drawAsPoints);
				if (ImGui::Checkbox("Wireframe", &appSettings.wireframe)) {
					glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);
				}
				if (ImGui::Checkbox("Back-face culling", &appSettings.backFaceCulling)) {
					if (appSettings.backFaceCulling)
						glEnable(GL_CULL_FACE);
					else
						glDisable(GL_CULL_FACE);
				}
			}

			if (ImGui::CollapsingHeader("Edit Cube"))
			{
				ImGui::PushID(0);
				ImGui::DragFloat3("Position", &cube.transform.position.x, 0.1f);
				ImGui::DragFloat("Scale", &cube.size, 0.05);
				ImGui::PopID();
			}
			
			if (ImGui::CollapsingHeader("Edit Plane"))
			{
				ImGui::PushID(1);
				ImGui::DragFloat3("Position", &plane.transform.position.x, 0.1f);
				ImGui::DragFloat("Scale", &plane.size, 0.05);
				ImGui::DragInt("SubDivisions", &plane.subDivisions, 1, 3, 100);
				ImGui::PopID();
			}

			if (ImGui::CollapsingHeader("Edit Cylinder"))
			{
				ImGui::PushID(2);
				ImGui::DragFloat3("Position", &cylinder.transform.position.x, 0.1f);
				ImGui::DragFloat("Height", &cylinder.height, 0.05);
				ImGui::DragFloat("Radius", &cylinder.radius, 0.05);
				ImGui::DragInt("SubDivisions", &cylinder.subDivisions, 1, 3, 100);
				ImGui::PopID();
			}
			
			if (ImGui::CollapsingHeader("Edit Sphere"))
			{
				ImGui::PushID(3);
				ImGui::DragFloat3("Position", &sphere.transform.position.x, 0.1f);
				ImGui::DragFloat("Scale", &sphere.radius, 0.05);
				ImGui::DragInt("SubDivisions", &sphere.subDivisions, 1, 3, 100);
				ImGui::PopID();
			}

			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 3);
	camera.target = ew::Vec3(0);
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}



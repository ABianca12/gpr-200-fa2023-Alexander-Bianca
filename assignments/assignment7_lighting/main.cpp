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

using namespace std;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light
{
	ew::Vec3 color = { 1, 1, 1 };
	ew::Transform transform;
};

const int MAX_NUM_OF_LIGHTS = 4;
int numLights = MAX_NUM_OF_LIGHTS;

Light lightsArray[MAX_NUM_OF_LIGHTS];

struct Material
{
	float ambient = 0.1;
	float diffuse = 0.25;
	float specular = 1.0;
	float shine = 128;
};

Material material;

bool blinnPhong = true;

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

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg", GL_REPEAT, GL_LINEAR);
	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));
	ew::Mesh lightSphere(ew::createSphere(0.1f, 16));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	resetCamera(camera,cameraController);

	lightsArray[0].transform.position = { 2, 1, 2 };
	lightsArray[0].color = { 1,0,0 };
	lightsArray[1].transform.position = { 2, 1, -2 };
	lightsArray[1].color = { 0,1,0 };
	lightsArray[2].transform.position = { -2, 1, -2 };
	lightsArray[2].color = { 0,0,1 };
	lightsArray[3].transform.position = { -2, 1, 2 };
	lightsArray[3].color = { 1,1,0 };

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < MAX_NUM_OF_LIGHTS; i++)
		{
			if (i < numLights)
			{
				shader.setVec3(("_LightsArray[" + to_string(i) + "].position"), lightsArray[i].transform.position);
				shader.setVec3(("_LightsArray[" + to_string(i) + "].color"), lightsArray[i].color);
			}
			else
			{
				shader.setVec3(("_LightsArray[" + std::to_string(i) + "].color"), ew::Vec3(0, 0, 0));
			}
		}

		shader.setFloat("_ambient", material.ambient);
		shader.setFloat("_diffuse", material.diffuse);
		shader.setFloat("_shine", material.shine);
		shader.setFloat("_specular", material.specular);
		shader.setVec3("_cameraPos", camera.position);
		shader.setBool("_blinnPhong", blinnPhong);

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//TODO: Render point lights
		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < numLights; i++)
		{
			unlitShader.setVec3("_Color", lightsArray[i].color);
			unlitShader.setMat4("_Model", lightsArray[i].transform.getModelMatrix());
			lightSphere.draw();
		}

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

			ImGui::ColorEdit3("BG color", &bgColor.x);

			ImGui::SliderInt("Number of lights", &numLights, 0, MAX_NUM_OF_LIGHTS);

			for (int i = 0; i < numLights; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Light"))
				{
					ImGui::DragFloat3("Position", &lightsArray[i].transform.position.x, 0.1);
					ImGui::ColorEdit3("Color", &lightsArray[i].color.x);
				}
				ImGui::PopID();
			}

			if (ImGui::CollapsingHeader("Lighting Settings"))
			{
				ImGui::SliderFloat("Ambient", &material.ambient, 0.0f, 1.0f);
				ImGui::SliderFloat("Diffuse", &material.diffuse, 0.0f, 1.0f);
				ImGui::SliderFloat("Shine", &material.shine, 2.0f, 1024.0f);
				ImGui::SliderFloat("Specular", &material.specular, 0.0f, 1.0f);
				ImGui::Checkbox("Blinn-Phong", &blinnPhong);
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
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}



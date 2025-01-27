#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/ewMath/vec3.h>
#include <ew/procGen.h>
#include <MyLibrary/transformations.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void moveCamera(GLFWwindow* window, MyLibrary::Camera* camera, MyLibrary::CameraControls* controls);

//Square aspect ratio for now. We will account for this with projection later.
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

int resolution[2] = { SCREEN_WIDTH, SCREEN_HEIGHT };

const int NUMOFCUBES = 4;

MyLibrary::Transform cubeTransformations[NUMOFCUBES];
ew::Vec3 rotationInDegrees = ew::Vec3(0.0f, 0.0f, 0.0f);

MyLibrary::Camera cam;
MyLibrary::CameraControls cameraControl;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures", NULL, NULL);
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
	cubeTransformations[0].position = ew::Vec3(0.5, 0.5);
	cubeTransformations[1].position = ew::Vec3(0.5, -0.5);
	cubeTransformations[2].position = ew::Vec3(-0.5, 0.5);
	cubeTransformations[3].position = ew::Vec3(-0.5, -0.5);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set uniforms
		for (size_t i = 0; i < NUMOFCUBES; i++)
		{
			shader.use();
			shader.setMat4("_Model", cubeTransformations[i].getModelMatrix());
			cubeMesh.draw();

			//Render UI
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			for (size_t i = 0; i < NUMOFCUBES; i++)
			{
				ImGui::PushID(i);

				if (ImGui::CollapsingHeader("Transformations"))
				{
					ImGui::DragFloat3("Position", &cubeTransformations[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransformations[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransformations[i].scale.x, 0.05f);
				}
				
				ImGui::PopID();
			}

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
}


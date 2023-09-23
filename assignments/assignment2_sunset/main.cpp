#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <MyLibrary/shader.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

MyLibrary::Vertex vertices[4] = {
	//x     y     z     u     v
	{-1.0, -1.0,  0.0,  0.0,  0.0}, //Bottom Left
	{ 1.0, -1.0,  0.0,  1.0,  0.0}, //Bottom Right
	{ 1.0,  1.0,  0.0,  1.0,  1.0}, //Top Right
	{-1.0,  1.0,  0.0,  0.0,  1.0}   //Top Left
};

unsigned int indicies[6] = {
	0, 1, 2,
	2, 3, 0
};

float resolution[2] = { SCREEN_WIDTH, SCREEN_HEIGHT };

float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;
bool showImGUIDemoWindow = true;

float sunSpeed = 1.8;
float hillSpeed = 1.8;
float stripesSpeed = 1.0;

float BGColor[3] = { 1.0, 1.0, 1.0 };
float sunColor[3] = { 1.0, 1.0, 0.0 };
float hillGrassColor[3] = { 0.0, 1.0, 0.0 };
float stripesColor[3] = { 0.5, 0.5, 0.5 };

bool drawWireframe = false;

int main()
{
	printf("Initializing...");
	if (!glfwInit())
	{
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL)
	{
		printf("GLFW failed to create window");
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	MyLibrary::Shader shader("assets/vertexShder.vert", "assets/fragmentShader.frag");
	shader.use();

	unsigned int vao = MyLibrary::createVAO(vertices, 4, indicies, 6);
	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		
		shader.setVec3("Sun Color", sunColor[0], sunColor[1], sunColor[2]);
		shader.setVec3("Hill Grass Color", hillGrassColor[0], hillGrassColor[1], hillGrassColor[2]);
		shader.setVec3("Stripes Color", stripesColor[0], stripesColor[1], stripesColor[2]);
		shader.setFloat("Sun Speed", sunSpeed);
		shader.setFloat("Hill Speed", hillSpeed);
		shader.setFloat("Stripes Speed", stripesSpeed);

		shader.setFloat("Time", glfwGetTime());

		if (drawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			// Create ImGui UI for shader parameters
			if (ImGui::CollapsingHeader("Speed Settings"))
			{
				ImGui::SliderFloat("Sun Speed", &sunSpeed, 0.1f, 2.0f);
				ImGui::SliderFloat("Hill Speed", &hillSpeed, 0.1f, 2.0f);
				ImGui::SliderFloat("Bars Speed", &stripesSpeed, 0.1f, 2.0f);
			}

			if (ImGui::CollapsingHeader("Color Settings"))
			{
				ImGui::ColorEdit3("Background Color", BGColor);
				ImGui::ColorEdit3("Sun Color", sunColor);
				ImGui::ColorEdit3("Sun Color", hillGrassColor);
				ImGui::ColorEdit3("Sun Color", stripesColor);
			}

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::ColorEdit3("Color", triangleColor);
			ImGui::SliderFloat("Brightness", &triangleBrightness, 0.0f, 1.0f);
			ImGui::End();
		

			if (showImGUIDemoWindow)
			{
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
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

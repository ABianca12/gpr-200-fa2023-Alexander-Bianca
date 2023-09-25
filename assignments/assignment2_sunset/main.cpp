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

MyLibrary::Vertex vertices[4] =
{
	//x     y     z     u     v
   {-1.0, -1.0,  0.0,  0.0,  0.0}, //Bottom Left
   { 1.0, -1.0,  0.0,  1.0,  0.0}, //Bottom Right
   { 1.0,  1.0,  0.0,  1.0,  1.0}, //Top Right
   {-1.0,  1.0,  0.0,  0.0,  1.0}   //Top Left
};

unsigned int indicies[6] =
{
	0, 1, 2,
	2, 3, 0
};

// BG Variables
float _BGTopColor[3] = { 1.0, 0.5, 0.8 };
float _BGBottomColor[3] = { 1.0, 0.6, 0.0 };

// Sun Variables
float _SunRisingColor[3] = { 1.0, 1.0, 0.0 };
float _SunSettingColor[3] = { 1.0, 0.0, 0.2 };
float _SunSpeed = 1.8;

// Hill Varibles
float _HillGrassColor[3] = { 0.0, 1.0, 0.0 };
float _Hill2Color[3] = { 0.9, 0.5, 0.0 };
float _HillSpeed = 1.8;

// Stripes Variables
float _StripesColor[3] = { 0.5, 0.5, 0.5 };
float _StripesSpeed = 1.8;

bool showImGUIDemoWindow = false;
bool drawWireframe = false;
float resolution[2] = { SCREEN_WIDTH, SCREEN_HEIGHT };

int main() {
	printf("Initializing...");
	if (!glfwInit())
	{
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fun With Fragment Shaders", NULL, NULL);
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

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	MyLibrary::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();

	unsigned int vao = MyLibrary::createVAO(vertices, 4, indicies, 6);
	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Set uniforms
		// BG uniforms
		shader.setVec3("_BGTopColor", _BGTopColor[0], _BGTopColor[1], _BGTopColor[2]);
		shader.setVec3("_BGBottomColor", _BGBottomColor[0], _BGBottomColor[1], _BGBottomColor[2]);

		// Sun uniforms
		shader.setVec3("_SunRisingColor", _SunRisingColor[0], _SunRisingColor[1], _SunRisingColor[2]);
		shader.setVec3("_SunSettingColor", _SunSettingColor[0], _SunSettingColor[1], _SunSettingColor[2]);
		shader.setFloat("_SunSpeed", _SunSpeed);

		// Hill uniforms
		shader.setVec3("_HillGrassColor", _HillGrassColor[0], _HillGrassColor[1], _HillGrassColor[2]);
		shader.setVec3("_Hill2Color", _Hill2Color[0], _Hill2Color[1], _Hill2Color[2]);
		shader.setFloat("_HillSpeed", _HillSpeed);

		// Stripes uniforms
		shader.setVec3("_StripesColor", _StripesColor[0], _StripesColor[1], _StripesColor[2]);
		shader.setFloat("_StripesSpeed", _StripesSpeed);

		shader.setVec2("_Resolution", SCREEN_WIDTH, SCREEN_HEIGHT);
		shader.setFloat("_Time", (float)glfwGetTime());

		if (drawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		// Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);

			if (ImGui::CollapsingHeader("Speed Settings"))
			{
				// Speed control
				ImGui::SliderFloat("Sun Speed", &_SunSpeed, 0.0f, 8.0f);
				ImGui::SliderFloat("Hill Speed", &_HillSpeed, 0.0f, 8.0f);
				ImGui::SliderFloat("Stripes Speed", &_StripesSpeed, 0.0f, 8.0f);
			}

			if (ImGui::CollapsingHeader("Color Settings"))
			{
				// BG color edits
				ImGui::ColorEdit3("Upper BG Color", _BGTopColor);
				ImGui::ColorEdit3("Lower BG Color", _BGBottomColor);

				// Sun color edits
				ImGui::ColorEdit3("Rising Sun Color", _SunRisingColor);
				ImGui::ColorEdit3("Setting Sun Color", _SunSettingColor);

				// Hill color edits
				ImGui::ColorEdit3("Hill Grass Color", _HillGrassColor);
				ImGui::ColorEdit3("Hill Color", _Hill2Color);

				// Stripes color edit
				ImGui::ColorEdit3("Stripes Color", _StripesColor);
			}

			if (ImGui::CollapsingHeader("Specs"))
			{
				ImGui::Text("Resolution: (%i) x (%i)", (int)resolution[0], (int)resolution[1]);
				ImGui::Text("Time: (%f)", glfwGetTime());
			}

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
	resolution[0] = width;
	resolution[1] = height;
}

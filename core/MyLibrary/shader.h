#ifndef SHADER_H
#define SHADER_H
#include "../ew/external/glad.h"
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include <string>

namespace MyLibrary
{
	struct Vertex
	{
		float x, y, z;
		float u, v;
	};

	class Shader
	{
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		void use();
		void setInt(const std::string& name, int v) const;
		void setFloat(const std::string& name, float v) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void Shader::setMat4(const std::string& name, const ew::Mat4& v) const;
	private:
		unsigned int m_id;
	};

	std::string loadShaderSourceFromFile(const std::string& filePath);
	unsigned int createShader(GLenum shaderType, const char* sourceCode);
	unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
	unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
}

#endif
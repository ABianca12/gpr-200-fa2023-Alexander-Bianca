#include "texture.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

unsigned int loadTexture(const char* filePath, int warpMode, int filterMode)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, numComponents;
	unsigned char* data = stbi_load(filePath, &width, &height, &numComponets, 0);

	if (data == NULL)
	{
		printf("Failed to load image %s", filePath);
		// Returns an array of bytes, width and height in pixels, and number of components per pixel
		stbi_image_free(data);

		return 0;
	}

	unsigned int texture;
	// Creates new texture name
	glGenTextures(1, &texture);
	// Binds texture name to GL_TEXTURE_2D alias
	glBindTexture(GL_TEXTURE_2D, texture);
	// Used to reserve memory and set texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	switch (numComponents)
	{
	case 1:
		format = GL_RED;
	case 2:
		format = GL_RG;
	case 3:
		format = GL_RGB;
	case 4:
		format = GL_RGBA;
	default:
		printf("Too many or too little components");
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	return texture;
}
#pragma once

unsigned int loadTexture(const char* filePath, int warpMode, int filterMode);

// Example use in main
//unsigned int brickTexture = loadTexture("assets/brick.png", GL_REPEAT, GL_LINEAR);
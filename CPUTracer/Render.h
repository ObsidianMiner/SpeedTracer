#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

#include "Utils.h"
#include "shaderClass.h"
#include "Text.h"

static void CheckError()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
		std::cerr << "GL Error: " << std::hex << err << std::endl;
}
static void SetupQuad(uint8_t* imageData, Vec3 windowSize, unsigned int* textureID, unsigned int* VAO)
{
	float vertices[] = {
		// Positions        // Texture Coords
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // Top-left
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f  // Top-right
	};
	unsigned int indices[] = {
	0, 1, 2, // First triangle
	2, 3, 0  // Second triangle
	};

	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		windowSize.X(), windowSize.Y(),
		0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

	glGenerateMipmap(GL_TEXTURE_2D);
	CheckError();

	unsigned int VBO, EBO;
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
static void RenderQuad(Vec3 windowSize, GLFWwindow* window, uint8_t* imageData, Shader gpuShader, Scene scene, bool rotate)
{
	if (windowSize.X() <= 0 || windowSize.Y() <= 0 || imageData == nullptr)
	{
		std::cout << "Invalid Data into RenderQuad()";
		return;
	}
	
	Shader cpuShader("CPUTracer/screen.vert", "CPUTracer/screen.frag");

	unsigned int textureID;
	unsigned int VAO;
	SetupQuad(imageData, windowSize, &textureID, &VAO);

	bool GPUMode = true;
	bool wasPressingMouse = false;

	Text::LoadFont();
	Shader textShader("text.vert", "text.frag");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(windowSize.X()), 0.0f, static_cast<float>(windowSize.Y()));
	textShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	float previousFPSGroupTime = glfwGetTime();
	int frameCounter = 0;
	int globalFrameCount = 0;
	std::string FPS = "";

	glfwSwapInterval(0);

	GLuint accumTexA, accumTexB, fboA, fboB;

	// Helper to make an accumulation texture
	auto createAccumTexture = [](GLuint& tex, GLuint& fbo, int width, int height) {
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "Framebuffer incomplete!" << std::endl;
		};

	createAccumTexture(accumTexA, fboA, windowSize.X(), windowSize.Y());
	createAccumTexture(accumTexB, fboB, windowSize.X(), windowSize.Y());

	GLuint currentFBO = fboA;
	GLuint currentAccumTex = accumTexA;
	GLuint prevAccumTex = accumTexB;
	int accumulationFrame = 1;
	bool resetAccum = false;

	Shader displayShader("GPUTracer/display.vert", "GPUTracer/display.frag");

	while (!glfwWindowShouldClose(window)) {
		globalFrameCount++;
		glfwPollEvents();
		if (glfwGetMouseButton(window, 0))
		{
			if (!wasPressingMouse)
			{
				std::cout << "Switched Modes" << std::endl;
				GPUMode = !GPUMode;
				wasPressingMouse = true;
			}
		}
		else wasPressingMouse = false;

		if (rotate)
		{
			scene.cameraRot += Vec3(0, 0.1, 0);
			resetAccum = true;
		}

		//Update FPS
		frameCounter++;
		float changeInTime = glfwGetTime() - previousFPSGroupTime;
		if (changeInTime > 0.35f)
		{
			FPS = GPUMode ? std::to_string((1.0 / changeInTime) * frameCounter) : "Pre-Rendered";
			frameCounter = 0;
			previousFPSGroupTime = glfwGetTime();
		}

		if (resetAccum)
		{
			accumulationFrame = 1;
			resetAccum = false;
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		if (GPUMode)
		{
			int framgeGenItterations = rotate ? 20 : 1;
			for (int i = 0; i < framgeGenItterations; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
				glViewport(0, 0, windowSize.X(), windowSize.Y());

				gpuShader.Activate();
				scene.UpdateBuffer(gpuShader, globalFrameCount, windowSize);


				// Bind previous accumulation texture
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, prevAccumTex);
				glUniform1i(glGetUniformLocation(gpuShader.ID, "prevFrame"), 1);

				// Pass frame count
				glUniform1i(glGetUniformLocation(gpuShader.ID, "frameCount"), accumulationFrame);

				// Render full screen quad
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				// Swap accumulation buffers
				std::swap(currentFBO, fboA == currentFBO ? fboB : fboA);
				std::swap(currentAccumTex, accumTexA == currentAccumTex ? accumTexB : accumTexA);
				std::swap(prevAccumTex, accumTexA == prevAccumTex ? accumTexB : accumTexA);

				// Blit final accumulated texture to screen
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glClear(GL_COLOR_BUFFER_BIT);
				displayShader.Activate(); // Shader that just draws a texture
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, currentAccumTex);
				glUniform1i(glGetUniformLocation(displayShader.ID, "screenTexture"), 0);
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				accumulationFrame++;
			}
		}
		else
		{
			cpuShader.Activate();
			int loc = glGetUniformLocation(cpuShader.ID, "screenTexture");
			glUniform1i(glGetUniformLocation(cpuShader.ID, "screenTexture"), 0); // Set texture uniform

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			accumulationFrame = 1;
		}
		
		
		Text::RenderText(textShader, "FPS: " + FPS, windowSize.X() / 8, windowSize.Y() / 8, 0.5f, LEFT_ALIGN, glm::vec3(1.0f));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	free(imageData);
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}
#endif
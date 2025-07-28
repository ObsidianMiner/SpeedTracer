#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "RenderedObject.h"
#include "MathUtil.h"
#include "Vec3.h"
#include "Sphere.h"
#include "Material.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <vector>

#include "shaderClass.h"

using std::make_shared;
using std::shared_ptr;

struct GMaterial
{
	glm::vec4 color;
	glm::vec4 emmisionColor;
	glm::vec4 specularColor;
	float smoothness;
	float specularProbability;
	int type;
	float padding;
};
struct GSphere
{
	glm::vec3 position;
	float radius;
	GMaterial mat;
};
class Scene : public RenderedObject
{
public:
	std::vector<shared_ptr<RenderedObject>> objects;
	Vec3 backgroundTopColor;
	Vec3 backgroundBottomColor;
	Vec3 cameraPos;
	Vec3 cameraRot;

	Scene() {}
	Scene(shared_ptr<RenderedObject> object) { Add(object); }

	void Clear() { objects.clear(); }

	void Add(shared_ptr<RenderedObject> object) { objects.push_back(object); }

	bool CheckHit(const Ray& r, Interval rayT, HitInfo& hit) const override
	{
		HitInfo tempHit;
		bool hasHit = false;
		double closestSoFar = rayT.max;
		
		for (const auto& object : objects)
		{
			if (object->CheckHit(r, Interval(rayT.min, closestSoFar), tempHit))
			{
				hasHit = true;
				closestSoFar = tempHit.t;
				hit = tempHit;
			}
		}
		return hasHit;
	}
	void CreateBuffer(Shader screenShader, Vec3 windowSize)
	{
		glGenBuffers(1, &sceneBuffer);
		UpdateBuffer(screenShader, 0, windowSize);
	}
	glm::vec3 VecToGlm(Vec3 vec3)
	{
		return glm::vec3(vec3.X(), vec3.Y(), vec3.Z());
	}
	void UpdateBuffer(Shader screenShader, int frameCount, Vec3 screenSize)
	{
		std::vector<GSphere> spheres;
		for (size_t i = 0; i < objects.size(); i++)
		{
			if (Sphere* sphere = dynamic_cast<Sphere*>(objects[i].get()))
			{
				glm::vec4 color(1.0);
				glm::vec4 emmision(0.0);
				float smoothness = 0.0;
				if (Lambertian* lambert = dynamic_cast<Lambertian*>(sphere->mat.get()))
				{
					color = glm::vec4(lambert->albedo.X(), lambert->albedo.Y(), lambert->albedo.Z(), 1.0);
				}
				if (Metal* lambert = dynamic_cast<Metal*>(sphere->mat.get()))
				{
					color = glm::vec4(lambert->albedo.X(), lambert->albedo.Y(), lambert->albedo.Z(), 1.0);
					smoothness = 0.7;
				}
				if (Emmisive* lambert = dynamic_cast<Emmisive*>(sphere->mat.get()))
				{
					color = glm::vec4(lambert->albedo.X(), lambert->albedo.Y(), lambert->albedo.Z(), 1.0);
					emmision = glm::vec4(lambert->emmision.X(), lambert->emmision.Y(), lambert->emmision.Z(), 1.0);

					smoothness = 0.8;
				}
				spheres.push_back(GSphere{ VecToGlm(sphere->center), float(sphere->radius), GMaterial{color, emmision, color, smoothness, smoothness, 0, 0} });
			}
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, sceneBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(GSphere), spheres.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sceneBuffer, 1);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		UpdateSphereCount(screenShader, spheres.size(), frameCount, screenSize);
	}
	void UpdateSphereCount(Shader screenShader, int count, int frameCount, Vec3 screenSize)
	{
		GLint backgroundTopColorLocation = glGetUniformLocation(screenShader.ID, "backgroundTopColor");
		glUniform3fv(backgroundTopColorLocation, 1, glm::value_ptr(VecToGlm(backgroundTopColor)));
		GLint backgroundBottomColorLocation = glGetUniformLocation(screenShader.ID, "backgroundBottomColor");
		glUniform3fv(backgroundBottomColorLocation, 1, glm::value_ptr(VecToGlm(backgroundBottomColor)));
		GLint screenParamsLocationLocation = glGetUniformLocation(screenShader.ID, "screenParams");
		glUniform3fv(screenParamsLocationLocation, 1, glm::value_ptr(VecToGlm(screenSize)));
		GLint camPosLocation = glGetUniformLocation(screenShader.ID, "camPos");
		glUniform3fv(camPosLocation, 1, glm::value_ptr(VecToGlm(cameraPos)));
		GLint camRotLocation = glGetUniformLocation(screenShader.ID, "camRot");
		glUniform3fv(camRotLocation, 1, glm::value_ptr(VecToGlm(cameraRot)));
		GLint frameCountLocation = glGetUniformLocation(screenShader.ID, "frameCount");
		glUniform1i(frameCountLocation, static_cast<GLuint>(frameCount));
		GLint sphereCountLocation = glGetUniformLocation(screenShader.ID, "sphereCount");
		if (sphereCountLocation != -1)
		{
			glUniform1i(sphereCountLocation, static_cast<GLuint>(count));
		}
		else
		{
			std::cout << "Warning: sphereCount uniform not found in shader";
		}
	}
	void ClearBuffer()
	{
		glDeleteBuffers(1, &sceneBuffer);
	}
private:
	GLuint sceneBuffer;
};
#endif
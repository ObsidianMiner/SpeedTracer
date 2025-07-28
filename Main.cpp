#include "Utils.h"
#include <chrono>
#include <thread>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "RenderedObject.h"
#include "Scene.h"
#include "Sphere.h"
#include "Camera.h"
#include "Render.h"

Scene TestScene()
{
	//Materials
	shared_ptr<Material> ground = make_shared<Lambertian>(Vec3(0.94, 0.94, 0.96));
	shared_ptr<Material> red = make_shared<Emmisive>(Vec3(0.2, 0.2, 0.2), Vec3(0.7, 0.2, 0.2));
	shared_ptr<Material> metalic = make_shared<Metal>(Vec3(0.8, 0.8, 0.8));
	shared_ptr<Material> black = make_shared<Lambertian>(Vec3(0.1, 0.1, 0.1));

	//Scene
	Scene scene;
	scene.backgroundTopColor = Vec3(0.0, 0.05, 0.1);
	scene.backgroundBottomColor = Vec3(0.0, 0.0, 0.0);
	scene.cameraPos = Vec3(0, 0, 0);

	scene.Add(make_shared<Sphere>(Vec3(0, -100.5, 1), 100.0, ground));
	scene.Add(make_shared<Sphere>(Vec3(0, 0, 1.5), 0.6, red));
	scene.Add(make_shared<Sphere>(Vec3(-1.3, 0, 1.5), 0.8, metalic));
	scene.Add(make_shared<Sphere>(Vec3(2, 0.7, 2), 0.5, black));
	return scene;
}
Scene SampleScene()
{
	//Materials
	shared_ptr<Material> material_ground = make_shared<Lambertian>(Vec3(0.8, 0.8, 0.8));
	shared_ptr<Material> material_center = make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5));
	shared_ptr<Material> material_left = make_shared<Metal>(Vec3(0.8, 0.8, 0.8));
	shared_ptr<Material> material_right = make_shared<Metal>(Vec3(0.8, 0.6, 0.2));

	//Scene
	Scene scene;
	scene.backgroundTopColor = Vec3(1.0, 1.0, 1.0);
	scene.backgroundBottomColor = Vec3(0.5, 0.7, 1.0);
	scene.cameraPos = Vec3(0, 0, 0);

	scene.Add(make_shared<Sphere>(Vec3(0, -100.5, 1), 100.0, material_ground));
	scene.Add(make_shared<Sphere>(Vec3(0, 0, 1.2), 0.5, material_center));
	scene.Add(make_shared<Sphere>(Vec3(-1.0, 0, 1.0), 0.5, material_left));
	scene.Add(make_shared<Sphere>(Vec3(1, 0.0, 1.0), 0.5, material_right));
	return scene;
}
Scene BasicScene()
{
	Scene scene;
	scene.backgroundTopColor = Vec3(1.0, 1.0, 1.0);
	scene.backgroundBottomColor = Vec3(0.5, 0.7, 1.0);
	scene.cameraPos = Vec3(0, 0, 0);
	shared_ptr<Material> material_ground = make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0));
	scene.Add(make_shared<Sphere>(Vec3(0, 0, 1.2), 0.5, material_ground));
	return scene;
}
Scene Room()
{
	Scene scene;
	scene.backgroundTopColor = Vec3(1,1,1);
	scene.backgroundBottomColor = Vec3(0.5,0.5,0.5);
	
	//Materials
	shared_ptr<Material> material_ground = make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0));
	shared_ptr<Material> blueGlow = make_shared<Emmisive>(Vec3(0.6, 0.6, 0.6), Vec3(0.2, 30.0, 30.0));

	//Walls
	scene.Add(make_shared<Sphere>(Vec3(0, 70, 1), 64, material_ground));
	scene.Add(make_shared<Sphere>(Vec3(0, -70, 1), 64, material_ground));
	scene.Add(make_shared<Sphere>(Vec3(70, 0, 1), 64, material_ground));
	scene.Add(make_shared<Sphere>(Vec3(-70, 0, 1), 64, material_ground));
	scene.Add(make_shared<Sphere>(Vec3(0, 0, 70), 64, material_ground));
	scene.Add(make_shared<Sphere>(Vec3(0, 0, -70), 64, material_ground));

	scene.Add(make_shared<Sphere>(Vec3(0, 0, 1.2), 0.5, blueGlow));
	return scene;
}
Scene LotsOBalls()
{
	Scene scene;
	scene.backgroundBottomColor = Vec3(0.01, 0.01, 0.01);
	scene.backgroundTopColor = Vec3(0.05, 0.05, 0.05);
	scene.cameraPos = Vec3(0, 0, 0);

	shared_ptr<Material> blueGlow = make_shared<Emmisive>(Vec3(0.6, 0.6, 0.6), Vec3(0.2, 0.2, 10.0));
	shared_ptr<Material> metalic = make_shared<Metal>(Vec3(0.8, 0.8, 0.8));
	shared_ptr<Material> white = make_shared<Lambertian>(Vec3(0.8, 0.8, 0.8));

	for (size_t i = 0; i < 4; i++)
	{
		scene.Add(make_shared<Sphere>(Vec3(RandomDouble() * 10 - 5, RandomDouble() * 10 - 5, 5 + RandomDouble() * 10), 0.5, blueGlow));
	}
	for (size_t i = 0; i < 4; i++)
	{
		scene.Add(make_shared<Sphere>(Vec3(RandomDouble() * 10 - 5, RandomDouble() * 10 - 5, 5 + RandomDouble() * 10), 0.5, metalic));
	}
	for (size_t i = 0; i < 8; i++)
	{
		scene.Add(make_shared<Sphere>(Vec3(RandomDouble() * 10 - 5, RandomDouble() * 10 - 5, 5 + RandomDouble() * 10), 0.5, white));
	}
	for (size_t i = 0; i < 8; i++)
	{
		scene.Add(make_shared<Sphere>(Vec3(RandomDouble() * 10 - 5, RandomDouble() * 10 - 5, 5 + RandomDouble() * 10), 0.5, make_shared<Lambertian>(Vec3(RandomDouble(), RandomDouble(), RandomDouble()))));
	}
	return scene;
}
int main()
{
	Vec3 windowSize(1920, 1080, 0);

	using namespace std::chrono_literals;
	auto start = std::chrono::high_resolution_clock::now();

	if (!glfwInit()) return -1;
	GLFWwindow* window = glfwCreateWindow(windowSize.X(), windowSize.Y(), "Speed Tracer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	const GLubyte* versionString = glGetString(GL_VERSION);
	if (versionString) {
		printf("OpenGL Version: %s\n", versionString);
	}
	else {
		printf("Failed to retrieve OpenGL version string.\n");
	}



	//Load Scene
	Scene scene = SampleScene();

	Shader gpuShader("GPUTracer/GPUscreen.vert", "GPUTracer/GPUscreen.frag");
	scene.CreateBuffer(gpuShader, windowSize);
	
	//Render
	Camera cam(windowSize.X(), windowSize.Y(), 3);
	uint8_t* imageData = cam.Render(scene);

	auto end = std::chrono::high_resolution_clock::now();
	auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "\nExecution Time: " << duration_ms.count() << " ms" << std::endl;

	RenderQuad(windowSize, window, imageData, gpuShader, scene, true);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
#ifndef CAMERA_H
#define CAMERA_H

#include "Utils.h"
#include "RenderedObject.h"
#include "Material.h"

#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <future>

class Camera
{
public:
	Camera(int imageWidth, int imageHeight, int samplesPerPixel) : imageWidth(imageWidth), imageHeight(imageHeight), samplesPerPixel(samplesPerPixel){}
	//Image
	const int imageWidth;

	const int imageHeight;
	double aspectRatio = 0.0;
	int samplesPerPixel = 10;
	int maxRays = 4;

	uint8_t* Render(RenderedObject& scene)
	{
		Init();

		uint8_t* imageData;
		imageData = (uint8_t*)malloc(imageWidth * imageHeight * 3 * sizeof(uint8_t));

		std::cout << std::thread::hardware_concurrency();

		const int total = imageHeight;
		const int threads = 8;
		int chunk = total / threads;

		std::vector<std::future<void>> futures;

		for (int t = 0; t < threads; ++t) {
			int start = t * chunk;
			int end = (t == threads - 1) ? total : start + chunk;

			futures.push_back(std::async(std::launch::async, &Camera::RenderRange, this, imageData, start, end, std::ref(scene)));
		}

		for (auto& f : futures) f.get();

		stbi_write_png("output.png", imageWidth, imageHeight, 3, imageData, imageWidth * 3);
		return imageData;
	}
	void RenderRange(uint8_t* imageData, int startHeight, int endHeight, RenderedObject& scene)
	{
		for (int j = startHeight; j < endHeight; j++)
		{
			//std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
			for (int i = 0; i < imageWidth; i++)
			{
				Vec3 color(0, 0, 0);
				for (int s = 0; s < samplesPerPixel; s++)
				{
					Ray r = GetRay(i, j);
					color += RayColor(r, maxRays, scene);
				}
				WriteColor(imageData, pixelSampleScale * color, i, j, imageWidth);
			}
		}
	}

private:

	//Camera
	double focalLength = 1.0;
	Vec3 cameraCenter = Vec3(0, 0, 0);

	Vec3 pixelDeltaU;
	Vec3 pixelDeltaV;
	Vec3 viewportTopLeft;
	Vec3 pixel00LOC;

	double pixelSampleScale;
	void Init()
	{
		aspectRatio = double(imageWidth) / imageHeight;

		double viewportHeight = 2.0;
		double viewPortWidth = viewportHeight * aspectRatio;

		Vec3 viewportU = Vec3(viewPortWidth, 0, 0);
		Vec3 viewportV = Vec3(0, -viewportHeight, 0);
		pixelDeltaU = viewportU / imageWidth;
		pixelDeltaV = viewportV / imageHeight;

		viewportTopLeft = cameraCenter + Vec3(0, 0, focalLength) - viewportU / 2 - viewportV / 2;
		pixel00LOC = viewportTopLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

		pixelSampleScale = 1.0 / samplesPerPixel;
	}
	static Vec3 RayColor(const Ray& r, int detph, const RenderedObject& scene)
	{
		if (detph <= 0) return Vec3(0, 0, 0);
		HitInfo hit;
		if (scene.CheckHit(r, Interval(0.003, infinity), hit))
		{
			Ray scattered;
			Vec3 attenuation;
			if (hit.mat->Scatter(r, hit, attenuation, scattered))
			{
				return attenuation * RayColor(scattered, detph - 1, scene);
			}
			return Vec3(0, 0, 0);
		}
		Vec3 unitDirection = Normalize(r.Direction());
		double a = 0.5 * (unitDirection.Y() + 1.0);
		return (1.0 - a) * Vec3(1.0, 1.0, 1.0) + a * Vec3(0.5, 0.7, 1.0);
	}
	Ray GetRay(int i, int j)
	{
		Vec3 offset = SampleSquare();
		Vec3 pixelSample = pixel00LOC + (i + offset.X()) * pixelDeltaU + (j + offset.Y()) * pixelDeltaV;
		return Ray(cameraCenter, pixelSample - cameraCenter);
	}
	Vec3 SampleSquare() const
	{
		return Vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0);
	}
};

#endif
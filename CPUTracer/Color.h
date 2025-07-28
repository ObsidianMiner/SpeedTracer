#pragma once

#include "Vec3.h"
#include "Interval.h"

#include <iostream>
#include <vector>

void PrintColor(std::ostream& out, const Vec3& color)
{
	double r = color.X();
	double g = color.Y();
	double b = color.Z();

	int rbyte = int(255.999 * r);
	int gbyte = int(255.999 * g);
	int bByte = int(255.999 * b);

	out << rbyte << ' ' << gbyte << ' ' << bByte << '\n';
}
void WriteColor(uint8_t imageData[], const Vec3& color, int u, int v, int imageWidth)
{
	double r = color.X();
	double g = color.Y();
	double b = color.Z();

	imageData[3 * (v * imageWidth + u)] = int(255.999 * r);
	imageData[3 * (v * imageWidth + u) + 1] = int(255.999 * g);
	imageData[3 * (v * imageWidth + u) + 2] = int(255.999 * b);
}
double LinearToGamma(double linearComponent)
{
	if (linearComponent > 0)
	{
		return std::sqrt(linearComponent);
	}
	return 0;
}
void WriteColor(std::vector<uint8_t> imageData, const Vec3& color, int u, int v, int imageWidth)
{
	double r = LinearToGamma(color.X());
	double g = LinearToGamma(color.Y());
	double b = LinearToGamma(color.Z());

	static const Interval intensity(0.000, 0.999);
	imageData[3 * (v * imageWidth + u)] = int(256 * intensity.Clamp(r));
	imageData[3 * (v * imageWidth + u) + 1] = int(256 * intensity.Clamp(g));
	imageData[3 * (v * imageWidth + u) + 2] = int(256 * intensity.Clamp(b));
}
#pragma once
#include <iostream>
#include <vector>
#include <limits>

#include "Vec3.h"

class Ray
{
public:
	Ray() {}

	Ray(const Vec3& origin, const Vec3& direction) : origin(origin), dir(direction)  {}

	const Vec3& Origin() const { return origin; };
	const Vec3& Direction() const { return dir; }

	Vec3 At(double v) const
	{
		return origin + v * dir;
	}
private:
	Vec3 origin;
	Vec3 dir;
};
double HitSphere(const Vec3& center, double radius, const Ray& r)
{
	Vec3 oc = center - r.Origin();
	auto a = r.Direction().LengthSquared();
	auto h = Dot(r.Direction(), oc);
	auto c = Dot(oc, oc) - radius * radius;
	auto discriminant = h * h - a * c;
	if (discriminant < 0) return -1.0;
	else return (h - std::sqrt(discriminant)) / a;
}
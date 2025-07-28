#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

#include "MathUtil.h"

class Vec3
{
public:
	double e[3];

	Vec3() : e{ 0,0,0 } {}
	Vec3(double x, double y, double z) : e{ x,y,z } {}

	double X() const { return e[0]; }
	double Y() const { return e[1]; }
	double Z() const { return e[2]; }

	Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	Vec3& operator*=(double v)
	{
		e[0] *= v;
		e[1] *= v;
		e[2] *= v;
		return *this;
	}
	Vec3& operator+=(const Vec3 v)
	{
		e[0] += v[0];
		e[1] += v[1];
		e[2] += v[2];
		return *this;
	}
	Vec3& operator/=(double v)
	{
		return *this *= 1 / v;
	}
	double Length() const
	{
		return std::sqrt(LengthSquared());
	}
	double LengthSquared() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}
	bool NearZero()
	{
		double s = 1e-8;
		return std::fabs(e[0]) < s && std::fabs(e[1]) < s && std::fabs(e[2]) < s;
	}
};
inline std::ostream& operator<<(std::ostream& out, const Vec3& v) { return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2]; }

inline Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3(a.e[0] + b.e[0], a.e[1] + b.e[1], a.e[2] + b.e[2]); }

inline Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3(a.e[0] - b.e[0], a.e[1] - b.e[1], a.e[2] - b.e[2]); }

inline Vec3 operator*(const Vec3& a, const Vec3& b) { return Vec3(a.e[0] * b.e[0], a.e[1] * b.e[1], a.e[2] * b.e[2]); }

inline Vec3 operator*(double v, const Vec3& vec) { return Vec3(v * vec.e[0], v * vec.e[1], v * vec.e[2]); }

inline Vec3 operator*(const Vec3& vec, double v) { return vec * v; }

inline Vec3 operator/(const Vec3& vec, double v) { return (1 / v) * vec; }

inline double Dot(const Vec3& a, const Vec3& b) {
	return a.e[0] * b.e[0]
		+ a.e[1] * b.e[1]
		+ a.e[2] * b.e[2];
}

inline Vec3 Cross(const Vec3& a, const Vec3& b) {
	return Vec3(a.e[1] * b.e[2] - a.e[2] * b.e[1],
		a.e[2] * b.e[0] - a.e[0] * b.e[2],
		a.e[0] * b.e[1] - a.e[1] * b.e[0]);
}

inline Vec3 Normalize(const Vec3& vec) {
	return vec / vec.Length();
}
static Vec3 RandomVec3()
{
	return Vec3(RandomDouble(), RandomDouble(), RandomDouble());
}
static Vec3 RandomVec3(double scaleMin, double scaleMax)
{
	return Vec3(RandomDouble(scaleMin, scaleMax), RandomDouble(scaleMin, scaleMax), RandomDouble(scaleMin, scaleMax));
}
inline Vec3 RandomUnitVector()
{
	while (true)
	{
		Vec3 p = RandomVec3();
		double lengthSquared = p.LengthSquared();
		if (1e-160 < lengthSquared && lengthSquared <= 1)
			return p / sqrt(lengthSquared);
	}
}
inline Vec3 RandomOnHemisphere(const Vec3& normal)
{
	Vec3 onUnitSphere = RandomUnitVector();
	if (Dot(onUnitSphere, normal) > 0.0)
		return onUnitSphere;
	else
		return -onUnitSphere;
}
inline Vec3 Reflect(const Vec3& v, const Vec3& normal)
{
	return v - Dot(v, normal) * normal;
}
#endif
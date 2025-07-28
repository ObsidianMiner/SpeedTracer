#ifndef MATERIAL_H
#define MATERIAL_H

#include "RenderedObject.h"

class Material
{
public:
	virtual ~Material() = default;

	virtual bool Scatter(const Ray& r, const HitInfo& hit, Vec3& attenuation, Ray& scattered) const
	{
		return false;
	}
};

class Lambertian : public Material
{
public:
	Vec3 albedo;
	Lambertian(const Vec3& albedo) : albedo(albedo) {}

	bool Scatter(const Ray& r, const HitInfo& hit, Vec3& attenuation, Ray& scattered) const override
	{
		Vec3 scatterDirection = hit.normal + RandomUnitVector();

		if (scatterDirection.NearZero()) scatterDirection = hit.normal;

		scattered = Ray(hit.p, scatterDirection);
		attenuation = albedo;
		return true;
	}
};

class Metal : public Material
{
public:
	Vec3 albedo;
	Metal(const Vec3& albedo) : albedo(albedo) {}
	
	bool Scatter(const Ray& r, const HitInfo& hit, Vec3& attenuation, Ray& scattered) const override
	{
		Vec3 reflected = Reflect(r.Direction(), hit.normal);
		scattered = Ray(hit.p, reflected);
		attenuation = albedo;
		return true;
	}
};
class Emmisive : public Material
{
public:
	Vec3 albedo;
	Vec3 emmision;
	Emmisive(const Vec3& albedo, const Vec3& emmision) : albedo(albedo), emmision(emmision) {}

	bool Scatter(const Ray& r, const HitInfo& hit, Vec3& attenuation, Ray& scattered) const override
	{
		Vec3 scatterDirection = hit.normal + RandomUnitVector();

		if (scatterDirection.NearZero()) scatterDirection = hit.normal;

		scattered = Ray(hit.p, scatterDirection);
		attenuation = albedo + emmision;
		return true;
	}
};

#endif
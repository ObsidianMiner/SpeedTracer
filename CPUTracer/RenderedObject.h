#ifndef HIT_H
#define HIT_H

#include "Vec3.h"
#include "Ray.h"
#include "Interval.h"

class Material;

class HitInfo
{
public:
	Vec3 p;
	Vec3 normal;
	double t;
	bool frontFace;
	shared_ptr<Material> mat;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="r"></param>
	/// <param name="outwardNormal">Must be normalized!</param>
	void SetFaceNormal(const Ray& r, const Vec3& outwardNormal)
	{
		frontFace = Dot(r.Direction(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};
class RenderedObject
{
public:
	virtual ~RenderedObject() = default;

	virtual bool CheckHit(const Ray& r, Interval rayT, HitInfo& hit) const = 0;
};
#endif
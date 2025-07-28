#ifndef SPHERE_H
#define SPHERE_H

#include "RenderedObject.h"

class Sphere : public RenderedObject
{
public:
    Vec3 center;
    double radius;
    shared_ptr<Material> mat;
    Sphere(const Vec3& center, double r, shared_ptr<Material> mat) : center(center), radius(std::fmax(0, r)), mat(mat)
    {

    }

    bool CheckHit(const Ray& r, Interval rayT, HitInfo& hit) const override
    {
        Vec3 oc = center - r.Origin();
        auto a = r.Direction().LengthSquared();
        auto h = Dot(r.Direction(), oc);
        auto c = oc.LengthSquared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!rayT.ContainsExclusive(root)) {
            root = (h + sqrtd) / a;
            if (!rayT.ContainsExclusive(root))
                return false;
        }

        hit.t = root;
        hit.p = r.At(hit.t);
        Vec3 outwardNormal = (hit.p - center) / radius;
        hit.SetFaceNormal(r, outwardNormal);
        hit.mat = mat;

        return true;
    }
private:
};
#endif
// ======================================================================
// Olio: Simple renderer
//
// ======================================================================

//! \file       ray.cc
//! \brief      Ray class

#include "core/ray.h"
#include <spdlog/spdlog.h>

namespace olio {
namespace core {

using namespace std;

    Ray::Ray(const Vec3r &origin, const Vec3r &dir) {
        this->origin_ = origin;
        this->dir_ = dir / dir.norm();
    }

    HitRecord::HitRecord(const Ray &ray, Real ray_t, const Vec3r &point, const Vec3r &face_normal) {
        this->ray_t_ = ray_t;
        this->point_ = point;
        this->SetNormal(ray, face_normal);
    }

}  // namespace core
}  // namespace olio

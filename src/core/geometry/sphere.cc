// ======================================================================
// Olio: Simple renderer
//
// ======================================================================

//! \file       sphere.cc
//! \brief      Sphere class

#include "core/geometry/sphere.h"
#include <spdlog/spdlog.h>
#include "core/ray.h"
#include <iostream>

namespace olio {
namespace core {

    //! \brief Constructor
    //! \param[in] name Node name
    Sphere::Sphere(const std::string &name) {
        this->name_ = name;
    }

    //! \brief Constructor
    //! \param[in] center Sphere position
    //! \param[in] radius Sphere radius
    //! \param[in] name Node name
    Sphere::Sphere(const Vec3r &center, Real radius, const std::string &name) {
            this->center_ = center;
            this->radius_ = radius;
            this->name_ = name;
            /*
            std::cout << "Sphere is at (" << center[0] << ", " << center[1] << ", " << center[2] << ") and has a"\
            << " radius of " << radius << std::endl;
            */
        }

    //! \brief Check if ray intersects with surface
    //! \details If the ray intersections the surface, the function
    //!          should fill in the 'hit_record' (i.e., information
    //!          about the hit point, normal, etc.)
    //! \param[in] ray Ray to check intersection against
    //! \param[in] tmin Minimum value for acceptable t (ray fractional distance)
    //! \param[in] tmax Maximum value for acceptable t (ray fractional distance)
    //! \param[out] hit_record Resulting hit record if ray intersected with surface
    //! \return True if ray intersected with surface
    bool Sphere::Hit(const Ray &ray, Real tmin, Real tmax,
        HitRecord &hit_record) {
            // Using formula from slide 22 0919
            Real discriminant = ((ray.GetDirection().dot(ray.GetOrigin() - this->center_)) * \
            (ray.GetDirection().dot(ray.GetOrigin() - this->center_))) - (\
            (ray.GetDirection().dot(ray.GetDirection())) * ((ray.GetOrigin() - this->center_).dot(\
            ray.GetOrigin() - this->center_) - (this->radius_ * this->radius_)));
            if (discriminant < 0) { // Lines do not intersect
                //std::cout << "No intersect " << discriminant << std::endl;
                return false;
            }
            Real tHit1 = (((-1 *ray.GetDirection()).dot(ray.GetOrigin() - this->center_)) - sqrt(discriminant)) / \
            (ray.GetDirection().dot(ray.GetDirection()));
            Real tHit2 = (((-1 *ray.GetDirection()).dot(ray.GetOrigin() - this->center_)) + sqrt(discriminant)) / \
            (ray.GetDirection().dot(ray.GetDirection()));
            Real wantedIntersect = std::min(tHit1, tHit2);
            //std::cout << "Sphere hit here " << wantedIntersect << std::endl;
            if (wantedIntersect < tmin || wantedIntersect > tmax) {
                // Intersect doesn't happen in range
                return false;
            }
                const Vec3r &hit_point = ray.At(wantedIntersect);
                hit_record.SetRayT(wantedIntersect);
                hit_record.SetPoint(hit_point);
                hit_record.SetNormal(ray, (hit_point - this->center_).normalized());
                hit_record.SetSurface(GetPtr());
                return true;
        }

    //! \brief Set sphere position
    //! \param[in] center Sphere center/position
    void Sphere::SetCenter(const Vec3r &center) {
        this->center_ = center;
    }

    //! \brief Set sphere radius
    //! \param[in] radius Sphere radius
    void Sphere::SetRadius(Real radius) {
        this->radius_ = radius;
    }

    //! \brief Get sphere position
    //! \return Sphere position
    Vec3r Sphere::GetCenter() const {
        return this->center_;
    }

    //! \brief Get sphere radius
    //! \return Sphere radius
    Real Sphere::GetRadius() const {
        return this->radius_;
    }

}  // namespace core
}  // namespace olio

// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2023 by Tyler Manrique
//
// Author: Tyler Manrique, 2023
// ======================================================================

//! \file       triangle.cc
//! \brief      Triangle class
//! \author     Tyler Manrique, 2022

#include "core/geometry/triangle.h"
#include <spdlog/spdlog.h>
#include "core/ray.h"

namespace olio {
namespace core {

    //! \brief Constructor
    //! \param[in] name Node name
    Triangle::Triangle(const std::string &name) {
        this->name_ = name;
    }

    //! \brief Constructor
    //! \details The constructor should also compute the face/triangle
    //! normal by calling ComputeNormal(). Points should be ordered
    //! counterclockwise wrt triangle normal.
    //! \param[in] point0 first triangle point
    //! \param[in] point1 second triangle point
    //! \param[in] point2 third triangle point
    //! \param[in] name Node name
    Triangle::Triangle(const Vec3r &point0, const Vec3r &point1, const Vec3r &point2,
        const std::string &name) {
            this->point0_ = point0;
            this->point1_ = point1;
            this->point2_ = point2;
            this->name_ = name;
            this->ComputeNormal();
        }
    //! \brief Compute/update triangle normal
    void Triangle::ComputeNormal() {
        this->normal_ = ((this->point1_ - this->point0_).cross(this->point2_ - this->point0_)) /\
        ((this->point1_ - this->point0_).cross(this->point2_ - this->point0_)).norm();//.normalize();
    }

    //! \brief Static function to compute ray-triangle intersection
    //!        between input ray and triangle defined by the three input
    //!        points.
    //! \param[in] p0 first triangle point
    //! \param[in] p1 second triangle point
    //! \param[in] p2 third triangle point
    //! \param[in] ray Input ray
    //! \param[in] tmin Minimum acceptable value for ray_t
    //! \param[in] tmax Maximum acceptable value for ray_t
    //! \param[out] In case of intersection, value of t for hit point p:
    //!             p = ray_origin + t * ray_dir
    //! \param[out] uv UV coordinates of the hit point inside the
    //!             triangle. Can be used to compute the barycentric
    //!             coordinates as: (1 - uv[0] - uv[1], uv[0], uv[1])
    //! \return True on success
    bool Triangle::RayTriangleHit(const Vec3r &p0, const Vec3r &p1, const Vec3r &p2,
                    const Ray &ray, Real tmin, Real tmax,
                    Real &ray_t, Vec2r &uv) {
                        Vec3r u = p1 - p0;
                        Vec3r v = p2 - p0;
                        Eigen::Matrix<Real, 3, 3> A;
                        A << u, v, (-1 * ray.GetDirection());
                        Vec3r b = ray.GetOrigin() - p0;
                        Vec3r x = A.colPivHouseholderQr().solve(b);
                        if (tmin > x[2] || tmax < x[2])
                            return false;
                        ray_t = x[2];
                        uv = {x[0], x[1]};
                        return true;
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
    bool Triangle::Hit(const Ray &ray, Real tmin, Real tmax,
        HitRecord &hit_record) {
            if (this->normal_.dot(ray.GetDirection()) == 0) // Never intersects
                return false;
            Real ray_t;
            Vec2r uv;
            if (this->RayTriangleHit(this->point0_, this->point1_, this->point2_,
            ray, tmin, tmax, ray_t, uv) == false)
                return false;

            // Check if in triangle
            Vec3r x = (ray_t * ray.GetDirection()) + ray.GetOrigin();
            if (((this->point1_ - this->point0_).cross(x - this->point0_)).dot(this->normal_) < 0)
                return false;
            if (((this->point2_ - this->point1_).cross(x - this->point1_)).dot(this->normal_) < 0)
                return false;
            if (((this->point0_ - this->point2_).cross(x - this->point2_)).dot(this->normal_) < 0)
                return false;
            hit_record.SetRayT(ray_t);
            hit_record.SetPoint(ray.At(ray_t));
            hit_record.SetNormal(ray, this->normal_);
            hit_record.SetSurface(GetPtr());
            return true;
        }

    //! \brief Set triangle points
    //! \details The function should also compute/update the triangle
    //!          normal by calling 'ComputeNormal()'
    //! \param[in] point0 first triangle point
    //! \param[in] point1 second triangle point
    //! \param[in] point2 third triangle point
    void Triangle::SetPoints(const Vec3r &point0, const Vec3r &point1, const Vec3r &point2) {
        this->point0_ = point0;
        this->point1_ = point1;
        this->point2_ = point2;
    }

    //! \brief Get triangle points
    //! \param[out] point0 first triangle point
    //! \param[out] point1 second triangle point
    //! \param[out] point2 third triangle point
    void Triangle::GetPoints(Vec3r &point0, Vec3r &point1, Vec3r &point2) const {
        point0 = this->point0_;
        point1 = this->point1_;
        point2 = this->point2_;
    }

    //! \brief Get triangle normal
    //! \return Triangle normal
    Vec3r Triangle::GetNormal() const {
        return this->normal_;
    }

}  // namespace core
}  // namespace olio

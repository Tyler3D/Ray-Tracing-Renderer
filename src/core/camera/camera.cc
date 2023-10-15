// ======================================================================
// Olio: Simple renderer
//
// ======================================================================

//! \file       camera.cc
//! \brief      Camera class

#include "core/camera/camera.h"
#include <iostream>
#include <spdlog/spdlog.h>

namespace olio {
namespace core {

using namespace std;

    Camera::Camera(const std::string &name) {
        this->name_ = name;
    }

    //! \brief Constructor
    //! \details Set up the camera using the input (LookAt) parameters.
    //! \param[in] eye Eye (center of projection) position
    //! \param[in] target Target/reference position for camera to look at
    //! \param[in] up The up vector
    //! \param[in] fovy y field of view in degrees
    //! \param[in] aspect viewport aspect ratio
    //! \param[in] name Node name
    Camera::Camera(const Vec3r &eye, const Vec3r &target, const Vec3r &up,
        Real fovy, Real aspect, const std::string &name) {
            this->fovy_ = fovy;
            this->aspect_ = aspect;
            this->name_ = name;
            //std::cout << "Fovy is " << fovy << " and aspect is " << aspect << std::endl;
            this->LookAt(eye, target, up, true);
        }

    //! \brief Set up camera using the provided LookAt parameters
    //! \param[in] eye Eye (center of projection) position
    //! \param[in] target Target/reference position for camera to look at
    //! \param[in] up The up vector
    //! \param[in] update_viewport Whether to also compute/update the
    //!            viewport parameters
    void Camera::LookAt(const Vec3r &eye, const Vec3r &target, const Vec3r &up,
            bool update_viewport) {
                this->eye_ = eye;
                this->target_ = target;
                this->up_ = up;
                Vec3r w = (this->eye_ - this->target_);
                w = w / w.norm();
                Vec3r u = (this->up_.cross(w));
                u = u / u.norm();
                Vec3r v = w.cross(u);
                v = v / v.norm();
                this->camera_xform_.col(0).head<3>() = u;
                this->camera_xform_.col(1).head<3>() = v;
                this->camera_xform_.col(2).head<3>() = w;
                this->camera_xform_.col(3).head<3>() = this->eye_;
                /*
                std::cout << "Camera Up is (" << this->up_[0] <<  ", "<< \
                this->up_[1] << ", "<< this->up_[2] << ")" << std::endl;

                std::cout << "Camera eye is (" << this->eye_[0] <<  ", "<< \
                this->eye_[1] << ", "<< this->eye_[2] << ")" << std::endl;

                std::cout << "Camera is facing (" << this->target_[0] <<  ", "<< \
                this->target_[1] << ", "<< this->target_[2] << ")" << std::endl;
                
                std::cout << "Vector w (" << w[0] <<  ", "<< \
                w[1] << ", "<< w[2] << ")" << std::endl;

                std::cout << "Vector u (" << u[0] <<  ", "<< \
                u[1] << ", "<< u[2] << ")" << std::endl;

                std::cout << "Vector v (" << v[0] <<  ", "<< \
                v[1] << ", "<< v[2] << ")" << std::endl;
                */
                if (update_viewport)
                    this->UpdateViewport();
            }

    //! \brief Set vertical field of view
    //! \param[in] fovy Vertical field of view in degrees
    //! \param[in] update_viewport Whether to update viewport parameters
    void Camera::SetFovy(Real fovy, bool update_viewport) {
        this->fovy_ = fovy;
        if (update_viewport)
            this->UpdateViewport();
    }

    //! \brief Set viewport aspect ratio
    //! \param[in] aspect viewport aspect ratio
    //! \param[in] update_viewport Whether to update viewport parameters
    void Camera::SetAspect(Real aspect, bool update_viewport) {
        this->aspect_ = aspect;
        if (update_viewport)
            this->UpdateViewport();
    }

    //! \brief Get eye (cop) position
    //! \return Eye position
    Vec3r Camera::GetEye() const {
        return this->eye_;
    }

    //! \brief Get target/reference position
    //! \return Target position
    Vec3r Camera::GetTarget() const {
        return target_;
    }

    //! \brief Get up vector
    //! \return Up vector
    Vec3r Camera::GetUpVector() const {
        return up_;
    }

    //! \brief Get camera xform, which describes the eye position and
    //!        the u, v, w axes
    //! \return 4x4 camera matrix
    Mat4r Camera::GetCameraXform() const {
        return camera_xform_;
    }

    //! \brief Get the vertical field of view
    //! \return Vertical field of view in degrees
    Real Camera::GetFovy() const {
        return fovy_;
    }

    //! \brief Get the viewport aspect ratio
    //! \return Viewport aspect ratio
    Real Camera::GetAspectRatio() const {
        return aspect_;
    }

    //! \brief Update viewport parameters using current values of
    //!        camera_xform_, fovy_, and aspect_
    //! \details Parameters that will be updated by the function:
    //!    * horizontal_: viewport's horizontal axis
    //!    * vertical_: viewport's vertical axis
    //!    * lower_left_corner_: lower left corner of the viewport
    void Camera::UpdateViewport() {
        this->vertical_ = 2 * tan(this->fovy_ * kPi / 360) * this->camera_xform_.col(1).head<3>();
        this->horizontal_ = this->aspect_ * 2 * tan(this->fovy_ * kPi / 360) * this->camera_xform_.col(0).head<3>();
        this->lower_left_corner_ = this->eye_ - this->camera_xform_.col(2).head<3>() - 0.5 * (this->horizontal_ + this->vertical_);
        /*
        std::cout << "Camera Vertical (" << this->vertical_[0] <<  ", "<< \
        this->vertical_[1] << ", "<< this->vertical_[2] << ")" << std::endl;

        std::cout << "Camera Horizontal (" << this->horizontal_[0] <<  ", "<< \
        this->horizontal_[1] << ", "<< this->horizontal_[2] << ")" << std::endl;

        std::cout << "Camera Lower left (" << this->lower_left_corner_[0] <<  ", "<< \
        this->lower_left_corner_[1] << ", "<< this->lower_left_corner_[2] << ")" << std::endl;
        */
    }

}  // namespace core
}  // namespace olio

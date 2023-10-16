// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2022 by Hadi Fadaifard
// AmbientLight & PointLight classes
// Implemented by Tyler Manrique
//
// Authors: Hadi Fadaifard, 2022
// Tyler Manrique, 2023
// ======================================================================

//! \file       light.h
//! \brief      Light classes
//! \author     Hadi Fadaifard, 2022

#pragma once

#include <memory>
#include <string>
#include "core/types.h"
#include "core/node.h"

namespace olio {
namespace core {

class Ray;
class HitRecord;
class Surface;

//! \class Light
//! \brief Light class
class Light : public Node {
public:
  OLIO_NODE(Light)

  //! \brief Constructor
  //! \param[in] name Node name
  explicit Light(const std::string &name=std::string());

  //! \brief Illuminate a hit point by retrieving its Phong material
  //! and evaluating it with a call to `Material::Eval()`
  //! \param[in] hit_record Hit record for the point
  //! \param[in] view_vec View vector (points away from the surface)
  //! \return Total radiance leaving the point in the direction of
  //!         view_vec
  virtual Vec3r Illuminate(const HitRecord &hit_record, const Vec3r &view_vec) const;
protected:
};

class PointLight : public Light {
public:
    OLIO_NODE(PointLight)

    explicit PointLight(const std::string &name=std::string());

    PointLight(const Vec3r &position, const Vec3r &intensity, const std::string &name=std::string());

    inline void SetPosition(const Vec3r &position) {this->position_ = position;}
    
    inline void SetIntensity(const Vec3r &intensity) {this->intensity_ = intensity;}
    
    inline Vec3r GetPosition() {return this->position_;}
    
    inline Vec3r GetIntensity() {return this->intensity_;}

    Vec3r Illuminate(const HitRecord &hit_record, const Vec3r &view_vec) const override;

protected:
Vec3r position_{0, 0, 0};
Vec3r intensity_{0, 0, 0};
};

class AmbientLight : public Light {
public:
    OLIO_NODE(AmbientLight)

    explicit AmbientLight(const std::string &name=std::string());

    AmbientLight(const Vec3r &ambient, const std::string &name=std::string());
    
    inline void SetAmbient(Vec3r &ambient) {this->ambient_ = ambient;}

    inline Vec3r GetAmbient() {return ambient_;}

    Vec3r Illuminate(const HitRecord &hit_record, const Vec3r &view_vec) const override;

protected:
Vec3r ambient_;
};


}  // namespace core
}  // namespace olio

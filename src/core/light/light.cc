// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2022 by Tyler Manrique
//
// Author: Tyler Manrique, 2023
// ======================================================================

//! \file       light.cc
//! \brief      Light class
//! \author     Tyler Manrique, 2022

#include "core/light/light.h"
#include "core/geometry/surface.h"
#include "core/ray.h"
#include "core/material/phong_material.h"
#include <iostream>

namespace olio {
namespace core {

using namespace std;

Light::Light(const std::string &name) :
  Node{name}
{
  name_ = name.size() ? name : "Light";
}


Vec3r Light::Illuminate(const HitRecord &, const Vec3r &) const
{
  return Vec3r{0, 0, 0};
}

PointLight::PointLight(const std::string &name) {
  this->name_ = name;
}

PointLight::PointLight(const Vec3r &position, const Vec3r &intensity, const std::string &name) {
  this->name_ = name;
  this->position_ = position;
  this->intensity_ = intensity;
}

Vec3r PointLight::Illuminate(const HitRecord &hit_record, const Vec3r &view_vec) const {
  Real distanceSquared = (this->position_ - hit_record.GetPoint()).dot(\
  (this->position_ - hit_record.GetPoint()));
  Vec3r light_vec = (this->position_ -  hit_record.GetPoint()).normalized();
  Vec3r irradiance = this->intensity_ * \
  std::max(0.0, (hit_record.GetNormal().dot(light_vec) ) ) / distanceSquared;
  
  std::shared_ptr<Surface> surface = hit_record.GetSurface();
  if (!surface)
    return Vec3r{0, 0, 0};
  std::shared_ptr<PhongMaterial> material = std::dynamic_pointer_cast<PhongMaterial>(surface->GetMaterial());
  if (!material || material->GetName() != "PhongMaterial") {
    std::cout << "Wrong material" << std::endl;
    return Vec3r{0, 0, 0};
  }

  Vec3r attenuation = material->Evaluate(hit_record, light_vec, view_vec);

  //std::cout << "Point attenuation " << attenuation << std::endl;
  //std::cout << "Point irradiance " << irradiance << std::endl;
  
  return Vec3r{irradiance[0] * attenuation[0], irradiance[1] * attenuation[1],
  irradiance[2] * attenuation[2]};
  
  //return irradiance * attenuation;
}

AmbientLight::AmbientLight(const std::string &name) {
  this->name_ = name;
}

AmbientLight::AmbientLight(const Vec3r &ambient, const std::string &name) {
  this->name_ = name;
  this->ambient_ = ambient;
}

Vec3r AmbientLight::Illuminate(const HitRecord &hit_record, const Vec3r &view_vec) const {  
  std::shared_ptr<Surface> surface = hit_record.GetSurface();
  if (!surface)
    return Vec3r{0, 0, 0};
  std::shared_ptr<PhongMaterial> material = std::dynamic_pointer_cast<PhongMaterial>(surface->GetMaterial());
  if (!material || material->GetName() != "PhongMaterial") {
    std::cout << "Wrong material" << std::endl;
    return Vec3r{0, 0, 0};
  }

  Vec3r ambient = material->GetAmbient();
  //std::cout << "Ambient vec " << ambient << std::endl;
  return Vec3r{this->ambient_[0] * ambient[0], this->ambient_[1] * ambient[1],
  this->ambient_[2] * ambient[2]};

  //return this->ambient_ * material->GetAmbient();
}

}  // namespace core
}  // namespace olio

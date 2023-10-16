// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2023 by Tyler Manrique
//
// Author: Tyler Manrique, 2023
// ======================================================================

//! \file       phong_material.cc
//! \brief      PhongMaterial class
//! \author     Tyler Manrique, 2022

#include "core/material/phong_material.h"
#include <spdlog/spdlog.h>
#include "core/light/light.h"
#include <iostream>

namespace olio {
namespace core {

using namespace std;

PhongMaterial::PhongMaterial(const std::string &name) : Material{} {
  name_ = name.size() ? name : "PhongMaterial";
  SetDiffuse(Vec3r{0, 0, 0});
}


PhongMaterial::PhongMaterial(const Vec3r &ambient, const Vec3r &diffuse,
			     const Vec3r &specular, Real shininess,
			     const Vec3r &mirror, const std::string &name) :
  Material{} {
  name_ = name.size() ? name : "PhongMaterial";
  this->ambient_ = ambient;
  this->specular_ = specular;
  this->shininess_ = shininess;
  this->mirror_ = mirror;
  SetDiffuse(diffuse);
}


void
PhongMaterial::SetDiffuse(const Vec3r &diffuse)
{
  diffuse_ = diffuse;
}


Vec3r
PhongMaterial::Evaluate(const HitRecord &hit_record, const Vec3r &light_vec,
			const Vec3r &view_vec) const
{
    Vec3r biSect = (view_vec + light_vec).normalized();
    //std::cout << "Reflection " << hit_record.GetNormal().dot(biSect) << std::endl;
    //std::cout << "specular " << this->specular_ << std::endl;
    //std::cout << "shine " << this->shininess_ << std::endl;

    return this->diffuse_ + this->specular_ * std::pow(\
    std::max(0.0, hit_record.GetNormal().dot(biSect)), this->shininess_);
}

}  // namespace core
}  // namespace olio

// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2023 by Tyler Manrique
//
// Author: Tyler Manrique, 2023
// ======================================================================

//! \file       sphere_list.h
//! \brief      Sphere_list class
//! \author     Tyler Manrique, 2023

#pragma once

#include "core/geometry/surface.h"

namespace olio {
namespace core {

class SurfaceList : public Surface {
    public:
    OLIO_NODE(SurfaceList)

    explicit SurfaceList(const std::string &name=std::string());

    SurfaceList(const std::vector<Surface::Ptr> &surfaces, const std::string &name=std::string());

    bool Hit(const Ray &ray, Real tmin, Real tmax, HitRecord &hit_record) override;

    protected:
    std::vector<Surface::Ptr> surfaces_{};
};

}
}
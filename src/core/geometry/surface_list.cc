// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2023 by Tyler Manrique
//
// Author: Tyler Manrique, 2023
// ======================================================================

//! \file       sphere_list.cc
//! \brief      Sphere_list class
//! \author     Tyler Manrique, 2023

#include "core/geometry/surface_list.h"
#include "core/ray.h"

namespace olio {
namespace core {

SurfaceList::SurfaceList(const std::string &name) {
    this->name_ = name.size() ? name : "SurfaceList";
}

SurfaceList::SurfaceList(const std::vector<Surface::Ptr> &surfaces, const std::string &name) {
    this->surfaces_ = surfaces;
    this->name_ = name.size() ? name : "SurfaceList";
}

bool SurfaceList::Hit(const Ray &ray, Real tmin, Real tmax, HitRecord &hit_record) {
    bool hitAny = false;
    hit_record.SetRayT(tmax);
    HitRecord bestRecord;

    for (auto surface : this->surfaces_) {
        if (surface->Hit(ray, tmin, tmax, bestRecord)) {
            hitAny = true;
            if (bestRecord.GetRayT() < hit_record.GetRayT()) {
                hit_record.SetRayT(bestRecord.GetRayT());
                hit_record.SetPoint(bestRecord.GetPoint());
                hit_record.SetNormal(ray, bestRecord.GetNormal());
                hit_record.SetSurface(bestRecord.GetSurface());
            }
        }
    }
    return hitAny;
}

}
}
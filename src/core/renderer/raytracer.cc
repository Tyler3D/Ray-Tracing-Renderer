// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2022 by Hadi Fadaifard
//
// Authors: Hadi Fadaifard, 2022
// Tyler Manrique, 2023
// ======================================================================

//! \file       raytracer.cc
//! \brief      Raytracer class
//! \author     Hadi Fadaifard, 2022

#include "core/renderer/raytracer.h"
#include <algorithm>
#include <chrono>
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>
#include "core/geometry/sphere.h"

namespace olio {
namespace core {

using namespace std;

bool
RayTracer::RayColor(const Ray &ray, Surface::Ptr scene, const std::vector<Light::Ptr> &lights, Vec3r &ray_color)
{
    // check whether ray hits any scene object
    ray_color = Vec3r{0, 0, 0};
    HitRecord hit_record;
    if (!scene->Hit(ray, kEpsilon, kInfinity, hit_record))
        return false;
      std::shared_ptr<PhongMaterial> material = \
      std::dynamic_pointer_cast<PhongMaterial>(hit_record.GetSurface()->GetMaterial());
    if (material && material->GetName() == "PhongMaterial") {
        for (Light::Ptr light : lights) {
            //std::cout << "Adding Light" << std::endl;
            ray_color += light->Illuminate(hit_record, -1 * ray.GetDirection());
        }
    } else {
      // Legacy no material system
      // Mark everything as red
      ray_color = {1, 0, 0};
    }
    return true;
}


bool
RayTracer::Render(Surface::Ptr scene, const std::vector<Light::Ptr> &lights, Camera::Ptr camera)
{
  // error checking
  if (!scene || !camera)
    return false;

  // start timer
  auto start_time = chrono::system_clock::now();

  // compute output image dimensions
  auto aspect = camera->GetAspectRatio();
  auto height = static_cast<int>(image_height_);
  auto width = static_cast<int>(aspect * static_cast<Real>(height) + 0.5f);
  if (height <= 0 || width <= 0) {
    spdlog::error("RayTracer: invalid image dimensions");
    return false;
  }

  // initialize image
  rendered_image_ = cv::Mat(cv::Size(width, height), CV_64FC3,
			    cv::Scalar(0, 0, 0));

  // start progress bar
  spdlog::info("Rendering...");
  auto total_pixels = static_cast<size_t>(width * height);
  RenderProgressStart(total_pixels);

  // ======================================================================
  //   1. Use the camera class to generate a ray that passes through the pixel
  //   2. Call RayColor() compute the color of the pixel
  //   3. Set pixel's color in rendered_image_
  //   4. Update the progress bar by calling RenderProgressIncDonePixels()
  // ======================================================================
  for (int y = 0; y < rendered_image_.rows; y++) {
    for (int x = 0; x < rendered_image_.cols; x++) {
      auto &pixel = rendered_image_.at<cv::Vec3d>(y, x);
      Ray ray = camera->GetRay(((Real) x) / rendered_image_.cols, ((Real) y) / rendered_image_.rows);
      Vec3r pix;
      this->RayColor(ray, scene, lights, pix);
      pixel[0] = pix[0];
      pixel[1] = pix[1];
      pixel[2] = pix[2];
      this->RenderProgressIncDonePixels();
    }
  }

  // stop progress bar
  RenderProgressEnd();

  // stop timer
  auto end_time = std::chrono::system_clock::now();
  auto total_time = chrono::duration_cast<chrono::duration<double>>
    (end_time - start_time).count();
  spdlog::info("Total render time: {}", total_time);

  return true;
}


cv::Mat
RayTracer::GammaCorrectImage(const cv::Mat &in_image, Real gamma) const
{
  Real gamma_inv = 1.0f / gamma;
  cv::Mat out_image(in_image.rows, in_image.cols, CV_64FC3);
  for (int y = 0; y < out_image.rows; ++y) {
    for (int x = 0; x < out_image.cols; ++x) {
      auto pixel = in_image.at<cv::Vec3d>(y, x);
      auto r = pow(pixel[0], gamma_inv);
      auto g = pow(pixel[1], gamma_inv);
      auto b = pow(pixel[2], gamma_inv);
      out_image.at<cv::Vec3d>(y, x) = cv::Vec3d{r, g, b};
    }
  }
  return out_image;
}


cv::Mat
RayTracer::RGBToBGRUChar(const cv::Mat &in_image) const
{
  // double RGB to uchar BGR:
  cv::Mat out_image(in_image.rows, in_image.cols, CV_8UC3);
  for (int y = 0; y < out_image.rows; ++y) {
    for (int x = 0; x < out_image.cols; ++x) {
      auto colord = in_image.at<cv::Vec3d>(y, x);
      out_image.at<cv::Vec3b>(y, x) =
	cv::Vec3b{static_cast<uchar>(CLAMP(colord[2] * 255 + 0.5f, 0, 255)),
		  static_cast<uchar>(CLAMP(colord[1] * 255 + 0.5f, 0, 255)),
		  static_cast<uchar>(CLAMP(colord[0] * 255 + 0.5f, 0, 255))};

    }
  }
  return out_image;
}


cv::Mat
RayTracer::RGBToBGRFloat32(const cv::Mat &in_image) const
{
  // double RGB to float BGR:
  cv::Mat out_image(in_image.rows, in_image.cols, CV_32FC3);
  for (int y = 0; y < out_image.rows; ++y) {
    for (int x = 0; x < out_image.cols; ++x) {
      auto colord = in_image.at<cv::Vec3d>(y, x);
      out_image.at<cv::Vec3f>(y, x) =
	cv::Vec3f{static_cast<float>(colord[2]),
		  static_cast<float>(colord[1]),
		  static_cast<float>(colord[0])};
    }
  }
  return out_image;
}


bool
RayTracer::WriteImage(const std::string &image_name, Real gamma) const
{
  namespace fs = boost::filesystem;

  // check we have a rendered image
  if (rendered_image_.empty())
    return false;

  // decide the output format
  bool is_exr = fs::path(image_name).extension().string() == ".exr";
  cv::Mat out_image;
  if (is_exr) {
    // HDR output does not need gamma correction
    out_image = RGBToBGRFloat32(rendered_image_);
  } else {
    // gamma correct and convert to uchar
    cv::Mat image_real;
    if (gamma == 1)
      image_real = rendered_image_;
    else
      image_real = GammaCorrectImage(rendered_image_, gamma);
    out_image = RGBToBGRUChar(image_real);
  }

  // write image
  cv::imwrite(image_name, out_image);
  return true;
}


void
RayTracer::RenderProgressStart(size_t total_pixels)
{
  const std::lock_guard<std::mutex> lock(progress_bar_mutex_);
  progress_bar_ = make_shared<tqdm>();
  progress_bar_done_pixels_  = 0;
  progress_bar_total_pixels_ = total_pixels;
}


void
RayTracer::RenderProgressIncDonePixels()
{
  const std::lock_guard<std::mutex> lock(progress_bar_mutex_);
  progress_bar_done_pixels_ = std::min(progress_bar_done_pixels_ + 1,
				       progress_bar_total_pixels_);
  if (!progress_bar_)
    return;
  progress_bar_->progress(static_cast<int>(progress_bar_done_pixels_),
			  static_cast<int>(progress_bar_total_pixels_));
}


void
RayTracer::RenderProgressEnd()
{
  const std::lock_guard<std::mutex> lock(progress_bar_mutex_);
  if (progress_bar_)
    progress_bar_->finish();
  progress_bar_.reset();
}

}  // namespace core
}  // namespace olio

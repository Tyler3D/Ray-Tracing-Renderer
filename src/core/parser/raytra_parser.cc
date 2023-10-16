#include "raytra_parser.h"
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>
#include "core/geometry/surface.h"
#include "core/geometry/sphere.h"
#include "core/camera/camera.h"
#include "core/geometry/triangle.h"
#include "core/geometry/surface_list.h"
#include "core/material/phong_material.h"

namespace olio {
namespace core {

using namespace std;
using boost::algorithm::trim;
namespace fs = boost::filesystem;

bool RaytraParser::ParseFile(const std::string &filename, Surface::Ptr &scene,
			     std::vector<Light::Ptr> &lights, Camera::Ptr &camera, Vec2i &image_size)
{
  // get absoulte file path
  fs::path filepath(filename);
  if (!filepath.is_absolute())
    filepath = boost::filesystem::absolute(filepath,
      boost::filesystem::current_path());
  fs::path path_prefix(filepath.parent_path());

  // check if file exists
  if (!fs::exists(filepath)) {
    spdlog::error("RaytraParser::ParseFile: file {} does not exist", filename);
    return false;
  }

  // open file
  ifstream in(filename);
  if (!in) {
    spdlog::error("RaytraParser::ParseFile: could not open file {} "
		  "for reading", filename);
    return false;
  }
  int camera_count = 0;
  int surface_count = 0;
  int ambient_light_count = 0;
  PhongMaterial::Ptr current_material = NULL;

  std::vector<Surface::Ptr> surface_list;

  // parse file
  for (string line; getline(in, line);) {
    // skip comments and empty lines
    trim(line);
    if (line.size() && line.back() == '\n')
      line.pop_back();
    if (!line.size() || line[0] == '/')
      continue;

    // parse file
    char cmd {'\0'};
    istringstream iss(line);
    iss >> cmd;
    switch(cmd) {
    case '/': continue;
    case 's':
      {
        // sphere
        if (!current_material) {
            spdlog::error("Need to declare a material before parsing surfaces");
          return false;
        }
        Real x, y, z, r;
        iss >> x >> y >> z >> r;
        Surface::Ptr sphere = Sphere::Create(Vec3r{x, y, z}, r);
        sphere->SetMaterial(current_material);
        surface_list.push_back(sphere);
        //scene = Sphere::Create(Vec3r{x, y, z}, r);
        ++surface_count;
        break;
      }
    case 'c':
      {
        // camera
        Real x, y, z, vx, vy, vz, focal_length, viewport_width, viewport_height,
          pixels_width, pixels_height;
        iss >> x >> y >> z >> vx >> vy >> vz >> focal_length >> viewport_width
            >> viewport_height >> pixels_width >> pixels_height;
        Vec3r eye{x, y, z};
        Vec3r view_vec{vx, vy, vz};
        view_vec.normalize();
        Vec3r target = eye + view_vec;
        Vec3r up_vec{0, 1, 0};
        if (view_vec.isApprox(up_vec))
          up_vec = Vec3r{0, 0, 1};
        Real fovy = 2 * atan2(viewport_height * 0.5f, focal_length) * kRADtoDEG;

        // check viewport/image aspect ratios
        Real viewport_aspect = viewport_width / viewport_height;
        if (!isfinite(viewport_aspect) || viewport_aspect <= 0) {
          spdlog::error("Camera has bad viewport_aspect ratio: {}",
            viewport_aspect);
          return false;
        }
        if (viewport_aspect > 20000) {
          spdlog::warn("Camera has very large viewport_aspect ratio: {}",
                viewport_aspect);
        }
        Real image_aspect = pixels_width / pixels_height;
        if (fabs(viewport_aspect - image_aspect) > kEpsilon) {
          spdlog::warn("Camera viewport has a different aspect ratio than "
                "output image (viewport_aspect: {} vs "
                "image_aspect: {})", viewport_aspect, image_aspect);
          spdlog::warn("Output image width will be adjusted to match the "
                "viewport aspect ratio");
        }

        // create camera
        camera = Camera::Create(eye, target, up_vec, fovy,viewport_aspect);
        image_size = Vec2i{static_cast<int>(pixels_width),
              static_cast<int>(pixels_height)};
        ++camera_count;
        break;
      }
    case 't':
      {
        // triangle
        if (!current_material) {
            spdlog::error("Need to declare a material before parsing surfaces");
          return false;
        }
        Real ax, ay, az, bx, by, bz, cx, cy, cz;
        iss >> ax >> ay >> az >> bx >> by >> bz >> cx >> cy >> cz;
        Surface::Ptr triangle = Triangle::Create(Vec3r{ax, ay, az}, Vec3r{bx, by, bz},
                                      Vec3r{cx, cy, cz});
        triangle->SetMaterial(current_material);
        surface_list.push_back(triangle);
        //scene = Triangle::Create(Vec3r{ax, ay, az}, Vec3r{bx, by, bz},
        //                               Vec3r{cx, cy, cz});
        ++surface_count;
	      break;
      }
    case 'l':
      {
        // Light
        char typeOfLight{'\0'};
        iss >> typeOfLight;
        switch (typeOfLight) {
            case 'a':
                {
                    // Ambient Light
                    Real r, g, b;
                    iss >> r >> g >> b;
                    Vec3r ambient{r, g, b};
                    lights.push_back(AmbientLight::Create(ambient));
                    std::cout << "Ambient Light source " << r << " " << g << " " << b << std::endl;
                    ++ambient_light_count;
                    break;
                }
            case 'p':
                {
                    // Point Light
                    Real x, y, z, r, g, b;
                    iss >> x >> y >> z >> r >> g >> b;
                    Vec3r pos{x, y, z};
                    Vec3r intensity{r, g, b};
                    std::cout << "Point Light source at " << x << " " << y << " " << z << " " << r << " " << g << " " << b << std::endl;
                    lights.push_back(PointLight::Create(pos, intensity));
                    break;
                }
            default:
                continue;
          }
          break;
        }
    case 'm':
        {
            // Material
            Real dr, dg, db, sr, sg, sb, r, ir, ig, ib;
            iss >> dr >> dg >> db >> sr >> sg >> sb >> r >> ir >> ig >> ib;
            Vec3r diffuse{dr, dg, db};
            Vec3r specular{sr, sg, sb};

            current_material = PhongMaterial::Create(diffuse, diffuse, specular, r);
            break;
        }
    default:
      continue;
    }
  }

  // close input file
  in.close();

  if (ambient_light_count > 1) {
    spdlog::error("Parse error: scene file should have at most one ambient light");
    return false;
  }

  if (camera_count != 1) {
    spdlog::error("Parse error: scene file should contain only one camera");
    return false;
  }

  //if (surface_count > 1) {
  //  spdlog::error("Parse error: scene file currently can only contain one "
	//	  "surface");
  //  return false;
  //}

  scene = SurfaceList::Create(surface_list);

  return true;
}

}  // namespace core
}  // namespace raytra

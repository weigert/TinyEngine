#ifndef TINYENGINE_PYTHON_UTIL
#define TINYENGINE_PYTHON_UTIL
// Util Class Definitions go Here

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "glm.hpp"

namespace py = pybind11;

#include <TinyEngine/camera>

void bind_util(py::module& module){

py::class_<glm::mat4>(module, "mat4")
  .def("__mul__", [](glm::mat4 lhs, glm::mat4 rhs){
    return lhs*rhs;
  });

auto cam_orthogonal = py::class_<Tiny::cam::orthogonal>(module, "cam_orthogonal");
cam_orthogonal.def(py::init<glm::vec2, glm::vec2, float>());
cam_orthogonal.def("hook", &Tiny::cam::orthogonal::hook);
cam_orthogonal.def("update", &Tiny::cam::orthogonal::update);
cam_orthogonal.def("proj", &Tiny::cam::orthogonal::proj);

auto cam_orbit = py::class_<Tiny::cam::orbit>(module, "cam_orbit");
cam_orbit.def(py::init<glm::vec3, glm::vec3>());
cam_orbit.def("hook", &Tiny::cam::orbit::hook);
cam_orbit.def("update", &Tiny::cam::orbit::update);
cam_orbit.def("view", &Tiny::cam::orbit::view);

using cam_ortho_orbit_t = Tiny::camera<Tiny::cam::orthogonal, Tiny::cam::orbit>;
auto cam_ortho_orbit = py::class_<cam_ortho_orbit_t>(module, "cam_ortho_orbit");
cam_ortho_orbit.def("vp", &cam_ortho_orbit_t::vp);
cam_ortho_orbit.def("hook", &cam_ortho_orbit_t::hook);
cam_ortho_orbit.def("update", &cam_ortho_orbit_t::update);

module.def("camera", [](const Tiny::cam::orthogonal& ortho, const Tiny::cam::orbit& orbit){
  return Tiny::camera(ortho, orbit);
}, py::keep_alive<0, 1>(), py::keep_alive<0, 2>());

}

#endif
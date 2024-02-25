#ifndef TINYENGINE_PYTHON
#define TINYENGINE_PYTHON

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "glm.hpp"
namespace py = pybind11;

// TinyEngine Python Bindings
// Nicholas McDonald 2024

// Binding Functions

void bind_core(py::module& module);
void bind_util(py::module& module);

#define TINYENGINE_CORE_BUFFER
#define TINYENGINE_CORE_INSTANCE
#define TINYENGINE_CORE_MODEL
#define TINYENGINE_CORE_SHADER
#define TINYENGINE_CORE_TARGET
#define TINYENGINE_CORE_TEXTURE

#include <TinyEngine/TinyEngine>

//
// TinyEngine Primary Interface
//

PYBIND11_MODULE(tinyengine, module){

module.doc() = "TinyEngine Python Module";

module.def("init", &Tiny::init);
module.def("window", &Tiny::window, py::arg("title"), py::arg("width"), py::arg("height"));
module.def("quit", &Tiny::quit);
module.def("loop", [](const std::function<void()>& f){    //Lambda Loop Pass
  Tiny::loop(f);
}, "Loop a Function");

//
// Event Class
//

auto Event = py::class_<Tiny::Event>(module, "Event");
Event.def(py::init<>());
Event.def_readonly("tick", &Tiny::Event::tick);
Event.def("process", &Tiny::Event::process);

//
// View Class
//

auto View = py::class_<Tiny::View>(module, "View");
View.def("pipeline", [](Tiny::View& view, std::function<void()> pipeline){
  view.pipeline = pipeline;
}, py::keep_alive<1, 2>());
View.def("target", &Tiny::View::target);

View.def_readwrite("vsync", &Tiny::View::vsync);
View.def_readwrite("linewidth", &Tiny::View::lineWidth);
View.def_readwrite("pointsize", &Tiny::View::pointSize);

// OpenGL Enumerator Hack

enum EnumMap{};
py::enum_<EnumMap> enummap(module, "GL");
enummap.value("lines", (EnumMap)GL_LINES);
enummap.value("points", (EnumMap)GL_POINTS);
enummap.value("triangles", (EnumMap)GL_TRIANGLES);
enummap.value("trianglestrip", (EnumMap)GL_TRIANGLE_STRIP);

module.attr("event") = &Tiny::event;
module.attr("view") = &Tiny::view;

bind_core(module);
bind_util(module);

}

#endif
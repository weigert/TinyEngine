#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include "glmcast.cpp"

PYBIND11_MODULE(tiny, module){

module.doc() = "TinyEngine Python Plugin";

//Core Functions and Variables

module.def("init", &Tiny::init, "Initialize TinyEngine");
module.def("window", &Tiny::window, "Open a TinyEngine Window",
          py::arg("windowName"), py::arg("width"), py::arg("height"));
module.def("quit", &Tiny::quit, "Shutdown TinyEngine");

module.def("loop", [](py::function f){    //Lambda Loop Pass
  Tiny::loop([&](){ f(); });
}, "Loop a Function");

module.def("benchmark", [](bool toggle){  //Lambda Setter
  Tiny::benchmark = toggle;
}, "Toggle Benchmark");

//Main Submodules

py::module view =  module.def_submodule("view");

  view.def("pipeline", [](const Handle& f){
    Tiny::view.pipeline = f;
    return;
  }, "Set View Pipeline");

  view.def("interface", [](const Handle& f){
    Tiny::view.interface = f;
    return;
  }, "Set View Interface");

  view.def("target", [](glm::vec3 color, bool clear){
    Tiny::view.target(color, clear);
    return;
  }, "Target Main View");

  // Settable Properties

  view.def("vsync", [](bool toggle){ Tiny::view.vsync = toggle; }, "Set vsync");
  view.def("linewidth", [](float set){ Tiny::view.lineWidth = set; }, "Set line width");
  view.def("pointsize", [](float set){ Tiny::view.pointSize = set; }, "Set point size");

py::module event =  module.def_submodule("event");

  event.def("handler", [](const Handle& f){
    Tiny::event.handler = f;
    return;
  }, "Set Event Handler");

//Utility Classes

//Shader Class

py::class_<ShaderBase>(module, "ShaderBase")
  .def(py::init<>())
  .def("use", &ShaderBase::use)
  .def("uniform", &ShaderBase::uniform<bool>)
  .def("uniform", &ShaderBase::uniform<int>)
  .def("uniform", &ShaderBase::uniform<float>)
  .def("uniform", &ShaderBase::uniform<glm::vec3>)
  .def("uniform", &ShaderBase::uniform<glm::mat4>);

py::class_<Shader, ShaderBase>(module, "Shader")
  .def(py::init<std::vector<std::string>>());

//Primitives and Models

py::class_<Primitive>(module, "Primitive")
  .def(py::init<>())
  .def("render", &Primitive::render,
       py::arg("mode") = GL_TRIANGLE_STRIP);

py::class_<Point, Primitive>(module, "Point")
  .def(py::init<>());

py::class_<Square2D, Primitive>(module, "Square2D")
  .def(py::init<>());

py::class_<Square3D, Primitive>(module, "Square3D")
  .def(py::init<>());

py::class_<Gizmo, Primitive>(module, "Gizmo")
  .def(py::init<>());

// Helper Namespace: Camera (Important)

py::module cam =  module.def_submodule("cam");

//Enum and Initializer

py::enum_<cam::camtype>(cam, "type")
  .value("PROJ", cam::PROJ)
  .value("ORTHO", cam::ORTHO);

cam.def("init", cam::init,
        py::arg("r") = 5.0f, py::arg("t") = cam::PROJ);

cam.def("handler", [](){
  cam::handler();
});

//Matrix Extraction
cam.def("vp", [](){
  return cam::vp;
});

// Property Setting

cam.def("zoomrate", [](float set){ cam::zoomrate = set; });
cam.def("near",     [](float set){ cam::near = set; });
cam.def("far",      [](float set){ cam::far = set; });

// OpenGL Enumerator Hack

enum EnumMap{};
py::enum_<EnumMap> enummap(module, "GL");
enummap.value("lines", (EnumMap)GL_LINES);
enummap.value("points", (EnumMap)GL_POINTS);
enummap.value("triangles", (EnumMap)GL_TRIANGLES);
enummap.value("trianglestrip", (EnumMap)GL_TRIANGLE_STRIP);

}

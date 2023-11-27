#ifndef TINYENGINE_PYTHON
#define TINYENGINE_PYTHON

// TinyEngine Python Bindings
// Nicholas McDonald 2023

#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include "glm.cpp"

PYBIND11_MODULE(tinyengine, module){

//
// TinyEngine Primarty Interface
//

module.doc() = "TinyEngine Python Module";

module.def("init", &Tiny::init);
module.def("window", &Tiny::window, py::arg("title"), py::arg("width"), py::arg("height"));
module.def("quit", &Tiny::quit);
module.def("loop", [](const std::function<void()>& f){    //Lambda Loop Pass
  Tiny::loop(f);
}, "Loop a Function");

//
// TinyEngine Global Static Core Classes
//

auto audio =  py::class_<Tiny::Audio>(module, "Audio");
auto event =  py::class_<Tiny::Event>(module, "Event");
auto view =   py::class_<Tiny::View>(module, "View");

module.attr("audio") = &Tiny::audio;
module.attr("event") = &Tiny::event;
module.attr("view") = &Tiny::view;

event.def_readwrite("handler", &Tiny::Event::handler);

view.def_readwrite("pipeline", &Tiny::View::pipeline);
view.def_readwrite("interface", &Tiny::View::interface);
view.def("target", &Tiny::View::target);

view.def_readwrite("vsync", &Tiny::View::vsync);
view.def_readwrite("linewidth", &Tiny::View::lineWidth);
view.def_readwrite("pointsize", &Tiny::View::pointSize);

//
// TinyEngine Core Classes
//

py::class_<Tiny::Buffer>(module, "Buffer")
 .def(py::init<>())
 .def("size", &Tiny::Buffer::size)
 .def("index", &Tiny::Buffer::index)
 // Value-Construction
 .def(py::init<const std::vector<int>&>())
 .def(py::init<const std::vector<float>&>())
 .def(py::init<const std::vector<uint32_t>&>())
 .def(py::init<const std::vector<glm::vec3>&>())
 .def(py::init<const std::vector<glm::vec4>&>())
 // Value Assignment
 .def("set", py::overload_cast<const std::vector<int>&>(&Tiny::Buffer::set<int>))
 .def("set", py::overload_cast<const std::vector<float>&>(&Tiny::Buffer::set<float>))
 .def("set", py::overload_cast<const std::vector<uint32_t>&>(&Tiny::Buffer::set<uint32_t>))
 .def("set", py::overload_cast<const std::vector<glm::vec3>&>(&Tiny::Buffer::set<glm::vec3>))
 .def("set", py::overload_cast<const std::vector<glm::vec4>&>(&Tiny::Buffer::set<glm::vec4>));
 // Value Retrieval
 //.def("get", py::overload_cast<std::vector<int>&>(&Tiny::Buffer::get<int>))
 //.def("get", py::overload_cast<std::vector<float>&>(&Tiny::Buffer::get<float>))
 //.def("get", py::overload_cast<std::vector<uint32_t>&>(&Tiny::Buffer::get<uint32_t>))
 //.def("get", py::overload_cast<std::vector<glm::vec3>&>(&Tiny::Buffer::get<glm::vec3>))
 //.def("get", py::overload_cast<std::vector<glm::vec4>&>(&Tiny::Buffer::get<glm::vec4>));

//Shader Class

py::class_<Tiny::ShaderBase>(module, "ShaderBase")
  .def(py::init<>())
  .def("use", &Tiny::ShaderBase::use)
  //Uniform Setters
  .def("uniform", &Tiny::ShaderBase::uniform<bool>)
  .def("uniform", &Tiny::ShaderBase::uniform<int>)
  .def("uniform", &Tiny::ShaderBase::uniform<float>)
  .def("uniform", &Tiny::ShaderBase::uniform<double>)
  .def("uniform", &Tiny::ShaderBase::uniform<glm::vec2>)
  .def("uniform", &Tiny::ShaderBase::uniform<glm::vec3>)
  .def("uniform", &Tiny::ShaderBase::uniform<glm::vec4>)
  .def("uniform", &Tiny::ShaderBase::uniform<glm::mat3>)
  .def("uniform", &Tiny::ShaderBase::uniform<glm::mat4>)
  //Texture Setters
  .def("texture", &Tiny::ShaderBase::texture<Tiny::Texture>)
  .def("texture", &Tiny::ShaderBase::texture<Tiny::Cubetexture>)
  //Buffering
  .def_static("ssbo", static_cast<void(*)(std::string)>(&Tiny::ShaderBase::ssbo), "Define an SSBO")
  .def_static("ssbo", static_cast<void(*)(std::vector<std::string>)>(&Tiny::ShaderBase::ssbo), "Define multiple SSBOs")
  .def("interface", static_cast<void(Tiny::ShaderBase::*)(std::string)>(&Tiny::ShaderBase::interface), "Add named SSBO to permitted interface blocks")
  .def("interface", static_cast<void(Tiny::ShaderBase::*)(std::vector<std::string>)>(&Tiny::ShaderBase::interface), "Add multiple named SSBOs to permitted interface blocks");

  //Two more Missing!

py::class_<Tiny::Shader, Tiny::ShaderBase>(module, "Shader")
  .def(py::init<std::vector<std::string>>())
  .def(py::init<std::vector<std::string>, std::vector<std::string>>());

//Primitives and Models

 // .def_readwrite("SIZE", &Tiny::Model::SIZE)
py::class_<Tiny::Model>(module, "Model")
  .def(py::init<>())
  .def(py::init<std::vector<std::string>>())
  .def("render", [](const Tiny::Model& model, const GLenum primitive){
    model.render(primitive, model.size());
  })
  .def("bind", [](Tiny::Model& model, const std::string name, Tiny::Buffer& buf){
    model.bind<glm::vec3>(name, buf);
  });
  //.def("render", py::overload_cast<const GLenum, const size_t>(&Tiny::Model::render));
  //.def("render", &Tiny::Model::render,
  //     py::arg("mode") = GL_TRIANGLE_STRIP)
  //.def("bind_vec3", &Tiny::Model::bind<glm::vec3>);

py::class_<Tiny::Point, Tiny::Model>(module, "Point")
  .def(py::init<>());

py::class_<Tiny::Square2D, Tiny::Model>(module, "Square2D")
  .def(py::init<>());

py::class_<Tiny::Square3D, Tiny::Model>(module, "Square3D")
  .def(py::init<>());

py::class_<Tiny::Gizmo, Tiny::Model>(module, "Gizmo")
  .def(py::init<>());


  //Enum and Initializer

  py::module cam =  module.def_submodule("cam");
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
  cam.def("moverate", [](float set){ cam::moverate = set; });
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

#endif
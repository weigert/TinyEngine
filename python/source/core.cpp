#ifndef TINYENGINE_PYTHON_CORE
#define TINYENGINE_PYTHON_CORE

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

//
// TinyEngine Core Classes
//

#include <TinyEngine/Buffer>
#include <TinyEngine/Model>
#include <TinyEngine/Shader>

void bind_core(py::module& module){

auto Buffer = py::class_<Tiny::Buffer>(module, "Buffer");
auto Model = py::class_<Tiny::Model>(module, "Model");

auto ShaderProgram = py::class_<Tiny::ShaderProgram>(module, "ShaderProgram");
auto Shader= py::class_<Tiny::Shader, Tiny::ShaderProgram>(module, "Shader");

//
// Buffer
//

Buffer.def(py::init<>());
Buffer.def(py::init<const std::vector<int>&>());
Buffer.def(py::init<const std::vector<float>&>());
Buffer.def(py::init<const std::vector<uint32_t>&>());
Buffer.def(py::init<const std::vector<glm::vec3>&>());
Buffer.def(py::init<const std::vector<glm::vec4>&>());

Buffer.def("size", &Tiny::Buffer::size);
Buffer.def("index", &Tiny::Buffer::index);

Buffer.def("set", py::overload_cast<const std::vector<int>&>(&Tiny::Buffer::set<int>));
Buffer.def("set", py::overload_cast<const std::vector<float>&>(&Tiny::Buffer::set<float>));
Buffer.def("set", py::overload_cast<const std::vector<uint32_t>&>(&Tiny::Buffer::set<uint32_t>));
Buffer.def("set", py::overload_cast<const std::vector<glm::vec3>&>(&Tiny::Buffer::set<glm::vec3>));
Buffer.def("set", py::overload_cast<const std::vector<glm::vec4>&>(&Tiny::Buffer::set<glm::vec4>));

// Buffer.def("get", py::overload_cast<std::vector<int>&>(&Tiny::Buffer::get<int>))
// Buffer.def("get", py::overload_cast<std::vector<float>&>(&Tiny::Buffer::get<float>))
// Buffer.def("get", py::overload_cast<std::vector<uint32_t>&>(&Tiny::Buffer::get<uint32_t>))
// Buffer.def("get", py::overload_cast<std::vector<glm::vec3>&>(&Tiny::Buffer::get<glm::vec3>))
// Buffer.def("get", py::overload_cast<std::vector<glm::vec4>&>(&Tiny::Buffer::get<glm::vec4>));

//
// Model
//

Model.def(py::init<const std::vector<std::string>>());

Model.def("render", [](const Tiny::Model& model, const GLenum primitive){
  model.render(primitive, model.size());
});

Model.def("bind", [](Tiny::Model& model, const std::string name, Tiny::Buffer& buf){
  model.bind<glm::vec3>(name, buf);
});

// Model Derivatives

py::class_<Tiny::Point, Tiny::Model>(module, "Point")
  .def(py::init<>());

py::class_<Tiny::Square2D, Tiny::Model>(module, "Square2D")
  .def(py::init<>());

py::class_<Tiny::Square3D, Tiny::Model>(module, "Square3D")
  .def(py::init<>());

py::class_<Tiny::Gizmo, Tiny::Model>(module, "Gizmo")
  .def(py::init<>());

//
// Shader
//

ShaderProgram.def(py::init<>());
ShaderProgram.def("use", &Tiny::ShaderProgram::use);

ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<bool>);
ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<int>);
ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<float>);
ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<double>);
ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<glm::vec2>);
ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<glm::vec3>);
ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<glm::vec4>);
ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<glm::mat3>);
ShaderProgram.def("uniform", &Tiny::ShaderProgram::uniform<glm::mat4>);

ShaderProgram.def("texture", &Tiny::ShaderProgram::texture<Tiny::Texture>);
ShaderProgram.def("texture", &Tiny::ShaderProgram::texture<Tiny::CubeTexture>);
ShaderProgram.def("storage", &Tiny::ShaderProgram::storage);

ShaderProgram.def("valid", &Tiny::ShaderProgram::valid);
ShaderProgram.def("index", &Tiny::ShaderProgram::index);
ShaderProgram.def("textures", &Tiny::ShaderProgram::textures);
ShaderProgram.def("storages", &Tiny::ShaderProgram::storages);

Shader.def(py::init<const std::vector<std::string>, const std::vector<std::string>>());

}

#endif

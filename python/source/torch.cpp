#include <TinyEngine/TinyEngine>
#include <torch/extension.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include "glmcast.cpp"

PYBIND11_MODULE(torch, module){

module.doc() = "TinyEngine pytorch Extension";

module.def("fill_tensor", [](Buffer buf, torch::Tensor T){
  buf.fill<float>((size_t)T.numel(), T.data_ptr<float>());
}, "Fill Buffer from pytorch Tensor");

}

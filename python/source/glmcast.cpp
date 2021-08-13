/*

  Casting of GLM Types from Python Lists

*/

#ifndef TINYENGINE_PYTHON_GLMCAST
#define TINYENGINE_PYTHON_GLMCAST

#include "glm/gtc/type_ptr.hpp" // includes all vector and matrix types too
#include <cstddef>

namespace pybind11 {
namespace detail {

template <>
struct type_caster<glm::vec3> {

  PYBIND11_TYPE_CASTER(glm::vec3, _("glm::vec3"));

  bool load(handle src, bool convert){

    auto s = reinterpret_steal<sequence>(src);
    if(s.size() != 3) return false;

    std::vector<float> v;
    for (auto it : s){
      make_caster<float> conv;
      if (!conv.load(it, convert))
        return false;
      v.push_back(cast_op<float &&>(std::move(conv)));
    }

    value = glm::make_vec3(&v[0]);
    return true;

	}

};

template <>
struct type_caster<glm::mat4> {

  PYBIND11_TYPE_CASTER(glm::mat4, _("glm::mat4"));

  bool load(handle src, bool convert){

    if (!isinstance<sequence>(src) || isinstance<bytes>(src) || isinstance<str>(src))
      return false;

    auto s = reinterpret_borrow<sequence>(src);
    if(s.size() != 16) return false;

    std::vector<float> v;
    for (auto it : s){
      make_caster<float> conv;
      if (!conv.load(it, convert))
        return false;
      v.push_back(cast_op<float &&>(std::move(conv)));
    }

    value = glm::make_mat4(&v[0]);
    return true;

	}

  static handle cast(const glm::mat4& src, return_value_policy /* policy */, handle /* parent */){

    py::list li;
    for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      li.append(src[i][j]);
    return li.release();

	}

};


}
}

#endif

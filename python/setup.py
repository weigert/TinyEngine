'''
TinyEngine Python Binding
Author: Nicholas McDonald, 2021

This is a setuptools installation script for generating the TinyEngine Module.
'''

import setuptools, pybind11
from pybind11.setup_helpers import build_ext

# Python Binding Configuration

__version__ = "0.1"

ext_modules = [

# Core Module
pybind11.setup_helpers.Pybind11Extension(
    name = "tinyengine.tiny",
    sources = ['source/core.cpp'],
    language = 'c++',
    include_dirs = ["/usr/local/include"],
    library_dirs = ["/usr/local/lib"],
    libraries=["TinyEngine", "X11", "GL", "pthread", "SDL2", "SDL2_image", "SDL2_mixer", "SDL2_ttf", "GLEW", "boost_system", "boost_filesystem"]
)

]

setuptools.setup(
    name = "tinyengine",
    version=__version__,
    author = "Nicholas McDonald",
    url = "https://github.com/weigert/TinyEngine",
    description = "tinyengine python binding",
    ext_modules = ext_modules,
    cmdclass={'build_ext': build_ext}
)

# TinyEngine

Note: These featuers are experimental.

## Python Binding

This is an attempt to make TinyEngine available in Python as a module using pybind11.

This is an experimental feature and intended as a learning project. Here I am learning how to make graphical OpenGL applications in C++ available in python as a module.

This could potentially open TinyEngine and the easy visualization of generated data with an elegant OpenGL boilerplate wrapper available to a broader audience.

### Building and Usage

First, make sure TinyEngine is installed normally.

To build the python bindings, install pybind11 and setuptools and run the setup script:

    pip3 install setuptools pybind11
    python3 setup.py install --user

The TinyEngine module should then be available for import regularly in python:

    from tinyengine import tiny

### Feature Availability

Currently, None


###

tinyengine
tinyengine.tiny

tinyengine.torch
tinyengine.numpy

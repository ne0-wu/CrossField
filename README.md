# Cross Fields

This project is a basic C++ implementation of N‐PolyVector Field algorithm as proposed by [Diamanti et al., 2019](https://onlinelibrary.wiley.com/doi/10.1111/cgf.12426). We implemented a basic version of the algorithm that computes the cross field of a triangle mesh. 

## How to build

This project uses `vcpkg` to manage its dependencies. To install the dependencies, run the following command:
```shell
$ vcpkg install eigen3 openmesh glad glfw3 imgui[glfw-binding,opengl3-binding] glm
```
To build the project, run the following commands:
```shell
$ mkdir build
$ cd build
$ cmake ..
```

## How to use

`main.cpp` contains an example of how to use the algorithm. The main function reads a triangle mesh from a file, computes the cross field, and visuliazes it using OpenGL.
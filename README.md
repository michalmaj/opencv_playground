# OpenCV Playground in Modern C++

A collection of small computer vision projects and demos written in modern C++ (C++23) using OpenCV 4.8.1.  
Built with [CMake](https://cmake.org/) and managed with [Conan 2.0](https://docs.conan.io/2/).

This repository is a personal playground for experimenting with OpenCV and related tools.  
Each subdirectory contains a self-contained demo with its own `main.cpp`.

---

## 🔧 Requirements

- C++ compiler with C++23 support (e.g. GCC 14+, Clang 19+)
- [CMake ≥ 3.20](https://cmake.org/download/)
- [Conan 2.0+](https://docs.conan.io/2/)
- Python 3.7+ (for Conan)

---

## ⚙️ Build Instructions

```bash
# Step 1: Create and enter build directory
mkdir -p build && cd build

# Step 2: Install dependencies via Conan
conan install .. --output-folder=. --build=missing

# Step 3: Configure with CMake using Conan's toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake

# Step 4: Build all demos
cmake --build .
```
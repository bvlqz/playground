# WebGL Playground

## Quick Start

### Setup Environment
```bash
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

### Build Project
```bash
rm -rf build
mkdir build
cd build
emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
emmake ninja

emmake ninja && python3 -m http.server 8000  --directory ../docs

```

## Technology Stack

### Core Technologies
- **C++**: Modern C++ with STL usage
- **WebAssembly**: Compiled via Emscripten
- **CMake**: Build system with Ninja generator

### Graphics & Window Management
- **OpenGL ES 3.0**: Targeting WebGL2
- **SDL2**: Window management and input handling

### UI Framework
- **Dear ImGui**: Immediate mode GUI
  - SDL2 backend integration
  - OpenGL ES 3 backend support

## Development Environment

### Build Tools
- Emscripten SDK (emsdk)
- emcmake and emmake for WebAssembly compilation
- Web platform targeting
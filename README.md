# imgui-wasm

Minimal imgui/WebAssembly/WebGL boilerplate, using CMake to build.

## Building for Web

Make sure you have cmake and emscripten installed.

```sh
mkdir build;
cd build;
emcmake cmake ..;
make;
```

## Building for MacOS

Make sure you've installed SDL2 on your system (for example, via `brew install sdl2`).

```sh
mkdir build;
cd build;
cmake .. -GXcode;
# ...open the generated xcode project...
```

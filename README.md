# Chord Engine

<img src="https://github.com/user-attachments/assets/eb783f61-a7c8-4060-96d0-16b92022b9d1" width="100" />

<img width="1918" height="1075" alt="image" src="https://github.com/user-attachments/assets/cb0ed745-2864-4f9d-8f25-285d9d50821e" />


Chord Engine is an experimental Vulkan renderer using [EnTT](https://github.com/skypjack/entt) for CPU-side scene data. This project is mainly for graphics programming, engine architecture experiments, and learning by building the pieces directly.

## Status

The engine is still actively changing. The editor and simulation targets are usable for development, but the API and project layout are not stable yet.

## Requirements

- CMake 3.26 or newer
- Ninja
- A C++23 compiler
- Vulkan SDK
- Git
- VS Code, optional but recommended

On Windows, LLVM/Clang is the compiler setup this repo is currently developed with. MSVC may work, but it is not the primary path right now.

The Vulkan SDK path must be passed to CMake through `VulkanSdkDir`. The SDK must include `glslc`. Slang shaders also require `slangc`; recent Vulkan SDK installs include it.

## Configure

Windows example:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DVulkanSdkDir="C:/VulkanSDK/1.4.341.1"
```

Linux example:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DVulkanSdkDir="$HOME/vulkan-sdk/x86_64"
```

Change `VulkanSdkDir` to match your installed SDK folder.

## Build

Build the editor:

```powershell
cmake --build build --target editor
```

Build and run the editor:

```powershell
cmake --build build --target run-editor
```

Build and run the simulation:

```powershell
cmake --build build --target run-simulation
```

Recompile engine shaders:

```powershell
cmake --build build --target engine_shaders
```

## VS Code Tasks

This repo includes tasks in `.vscode/tasks.json`.

Before running the configure task, edit the `-DVulkanSdkDir=...` argument in the task file if your Vulkan SDK is installed somewhere else.

Useful tasks:

- `configure-debug-windows`
- `configure-release-windows`
- `build-editor`
- `build-and-run-editor`
- `build-and-run-simulation`
- `recompile-engine-shaders`

Open the command palette and run `Tasks: Run Task`, then choose the task you want.

## Notes

- Build output is written to `build/`.
- Compiled SPIR-V shader files are ignored by Git.
- Third-party dependencies are fetched by CMake with `FetchContent`.

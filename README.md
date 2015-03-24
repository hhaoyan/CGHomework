# libOOGL: Object Oriented OpenGL Library

### Overview
libOOGL is a cross platform wrapper for OpenGL, which lets developers focus on their graphics rather than the OpenGL API!

### Usage
There XCode project is always expected to work, you could view it, study the example cases, and borrow codes to form your own OpenGL renderer!

However when you plan to start your own project, the source codes must be manually added to your IDE, source files under "src" should be compiled and header files under "inc" should be included to your project.

### Notes on Mac OSX
Those frameworks should be added to XCode to support libOOGL:

- Cocoa
- OpenGL
- CoreVideo
- IOKit
- libglfw3.a under directory "libglfw/osx_build"

If glfw library is broken on your machine, please consider build on your own:

```
$libOOGL(OSX): cd glfw/glfw-3.1.1
$glfw-3.1.1(OSX): cd osx_build
$osx_build(OSX): ./build.sh
$osx_build(OSX): 
```

now libglfw3.a should be updated and useable for your machine.
### Notes on Windows
If you plan to use Visual Studio, please add libglfw3.a under directory "libglfw/win_build" to your project.

### Libraries used
libOOGL uses [glm](https://github.com/g-truc/glm) and [glfw](http://www.glfw.org/), it's recommended to directly use header and source files from directory "glm" and "glfw" rather than download the latest, which may lead to some unexpected behaviors.
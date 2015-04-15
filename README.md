# libOOGL: Object Oriented OpenGL Library
##Note: Out of maintenance since Apr. 2015

### Overview
libOOGL is a cross platform wrapper for OpenGL, which lets developers focus on their graphics rather than the OpenGL API!
### Disclaimer

This project is started to support our group homework for Computer Graphics course (2015). With very limited time to write code, **it may be incomplete, buggy or even unable to build/run on your computer**, feel free to contact me if you have any ideas!

### Usage
The XCode project is always expected to work, you could view it, study the example cases, and borrow some codes to form your own OpenGL renderer!

However when you plan to start your own project, the source codes must be manually added to your IDE, source files under "src" should be compiled and header files under "inc" should be included to your project.

### Notes on Mac OSX
Those frameworks should be added to XCode to support libOOGL:

- Cocoa
- OpenGL
- CoreVideo
- IOKit
- libglfw3.a under directory "libglfw/osx_build"
- libfreetype.a under directory "freetype/osx_build"

If libraries are broken on your machine, please consider build on your own, this is the example for glfw:

```
$libOOGL(OSX): cd glfw/glfw-3.1.1
$glfw-3.1.1(OSX): cd osx_build
$osx_build(OSX): ./build.sh
$osx_build(OSX): 
```

now libglfw3.a should be updated and ready to be built with.
### Notes on Windows
If you plan to use Visual Studio, please add static libraries under directories "$(LIB_DIRECTORY)/win_build" to your project.

### Libraries used
libOOGL uses [glm](https://github.com/g-truc/glm), [glfw](http://www.glfw.org/), and [freetype](http://www.freetype.org), it's not recommended to use the latest versions downloaded from their websites, since the code change could lead to some unexpected behaviors.
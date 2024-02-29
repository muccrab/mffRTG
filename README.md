# gl_tutorials
Short demos and tutorials for OpenGL and other graphic libraries


## Loading libraries

[loading library overview](https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library)

### Why simple GL.h include does not suffice

Limited to OpenGL's Core Version: GL.h typically corresponds to OpenGL version 1.1, which is severely outdated. 
It doesn't include function pointers for modern OpenGL features or extensions.

Platform-Specific Function Pointers: OpenGL's design requires that function pointers for newer features be obtained at runtime. 
This is because different graphics drivers may implement different versions of OpenGL and support different sets of extensions. 
A static header file like GL.h cannot provide this level of flexibility.

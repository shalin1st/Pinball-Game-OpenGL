#ifndef _globalOpenGL_GLFW_HG_
#define _globalOpenGL_GLFW_HG_

// glad, glm, and GLFW need to be included in a certain order,
//	or the build is going to 'crash and burn'

// Including this file in anything that 'touches' anything related
//	to OpenGL will ensure that things are included in the correct order. 

// NOTE: glm does NOT require glad, GLFW, or gl.h, so you can include it 
//	anywhere you need it.

// SHORT VERSION OF THIS: If you need ANYTHING with OpenGL (constant, functions, whatever), 
//                         then include THIS file, not the glad and GLFW files specifically)

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// These are here as they are set within the GLFW call back. 
// (The thinking is that anything that sets these call backs would 
//  HAVE to include this file, anyway.)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void window_size_callback(GLFWwindow* window, int width, int height);

#include "Error/COpenGLError.h"


#endif

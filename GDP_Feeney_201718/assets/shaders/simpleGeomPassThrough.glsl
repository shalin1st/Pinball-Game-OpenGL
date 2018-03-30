// Geometry shader
// v 1.50 is GLSL for OpenGL 3.2 (yes, I realize that makes zero sense)
// (The next version for the geometry shader is 3.3)
#version 400 core


layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

in vec4 gColor[];				// was: vec4
in vec3 gVertNormal[];		// Also in "world" (no view or projection)
in vec3 gVecWorldPosition[];	// 
in vec4 gUV_X2[];			// Added: UV 1 and 2 to fragment
in vec3 gTangent[];		// For bump (or normal) mapping
in vec3 gBitangent[];	// For bump (or normal) mapping

out vec4 fColor;				// was: vec4
out vec3 fVertNormal;		// Also in "world" (no view or projection)
out vec3 fVecWorldPosition;	// 
out vec4 fUV_X2;			// Added: UV 1 and 2 to fragment
out vec3 fTangent;		// For bump (or normal) mapping
out vec3 fBitangent;	// For bump (or normal) mapping

void main()
{
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}

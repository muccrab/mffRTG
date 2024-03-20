#pragma once

#include "ogl_resource.hpp"
#include "obj_file_loading.hpp"

struct IndexedBuffer {
	OpenGLResource vbo;
	OpenGLResource ebo;
	OpenGLResource vao;

	unsigned int indexCount = 0;
	GLenum mode = GL_TRIANGLES;
};


IndexedBuffer
generateQuadTex();

IndexedBuffer
generateCubeOutlineBuffers();

IndexedBuffer
generateCubeBuffers();

IndexedBuffer
generateCubeBuffersNormTex();

IndexedBuffer
generateMeshBuffersNormTex(const ObjMesh &aMesh);

IndexedBuffer
generateQuadMeshBuffersNormTex(const ObjMesh &aMesh);

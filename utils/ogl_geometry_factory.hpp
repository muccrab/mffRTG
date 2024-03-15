#pragma once

#include <memory>
#include <map>
#include <string>
#include <filesystem>

#include "geometry_factory.hpp"
#include "ogl_resource.hpp"


namespace fs = std::filesystem;

struct IndexedBuffer {
	OpenGLResource vbo;
	OpenGLResource ebo;
	OpenGLResource vao;

	unsigned int indexCount = 0;
	GLenum mode = GL_TRIANGLES;
};


class OGLGeometry: public AGeometry {
public:
	OGLGeometry(IndexedBuffer buff) :
		buffer(std::move(buff))
	{}
	IndexedBuffer buffer;

	void bind() const {
		GL_CHECK(glBindVertexArray(buffer.vao.get()));
	}

	void draw() const {
  		GL_CHECK(glDrawElements(buffer.mode, buffer.indexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(0)));
	}
};

class OGLGeometryFactory: public GeometryFactory {
public:
	std::shared_ptr<AGeometry> getCube();
	std::shared_ptr<AGeometry> getCubeOutline();
	std::shared_ptr<AGeometry> getCubeNormTex();

	std::shared_ptr<AGeometry> loadMesh(fs::path aMeshPath, RenderStyle aRenderStyle);
protected:
	std::map<std::string, std::shared_ptr<OGLGeometry>> mObjects;
};

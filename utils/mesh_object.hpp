#pragma once

#include "scene_object.hpp"
#include "material_factory.hpp"

class MeshObject: public SceneObject {
public:
	MeshObject() {};

	void setMaterial(MaterialParameters aMaterialParams) {
		mMaterialParams = aMaterialParams;
	}
protected:
	MaterialParameters mMaterialParams;
	std::shared_ptr<AShaderProgram> mShaderProgram;
	std::shared_ptr<AGeometry> mGeometry;
};


#pragma once

#include "scene_object.hpp"
#include "material_factory.hpp"

class MeshObject: public SceneObject {
public:
	MeshObject() {};

	void setMaterial(MaterialParameters aMaterialParams) {
		mMaterialParams = aMaterialParams;
	}

	std::optional<RenderData> getRenderData(const RenderOptions &aOptions) const override {
		if (!mShaderProgram || !mGeometry) {
			return std::optional<RenderData>();
		}
		return std::optional<RenderData>(RenderData{
				getModelMatrix(),
				mMaterialParams,
				*mShaderProgram,
				*mGeometry
			});
	}
protected:
	MaterialParameters mMaterialParams;
	std::shared_ptr<AShaderProgram> mShaderProgram;
	std::shared_ptr<AGeometry> mGeometry;
};


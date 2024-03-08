#pragma once

#include "scene_object.hpp"
#include "material_factory.hpp"

struct RenderInfo {
	MaterialParameters materialParams;
	std::shared_ptr<AShaderProgram> shaderProgram;
	std::shared_ptr<AGeometry> geometry;
};

class MeshObject: public SceneObject {
public:
	MeshObject() {};

	void setMaterial(std::string aMode, MaterialParameters aMaterialParams) {
		mRenderInfos[aMode].materialParams = aMaterialParams;
	}

	std::optional<RenderData> getRenderData(const RenderOptions &aOptions) const override {
		auto it = mRenderInfos.find(aOptions.mode);
		if (it == mRenderInfos.end()) {
			return std::optional<RenderData>();
		}


		if (!it->second.shaderProgram || !it->second.geometry) {
			return std::optional<RenderData>();
		}
		return std::optional<RenderData>(RenderData{
				getModelMatrix(),
				it->second.materialParams,
				*(it->second.shaderProgram),
				*(it->second.geometry)
			});
	}
protected:
	std::map<std::string, RenderInfo> mRenderInfos;
	// MaterialParameters mMaterialParams;
	// std::shared_ptr<AShaderProgram> mShaderProgram;
	// std::shared_ptr<AGeometry> mGeometry;
};


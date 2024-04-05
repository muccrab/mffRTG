#pragma once

#include "vertex.hpp"
#include "mesh_object.hpp"

// TODO - finish for generic mesh and isntance attributes
class InstancedMeshObject: public MeshObject {
public:
	InstancedMeshObject(std::shared_ptr<MeshObject> aSourceMesh, std::vector<VertexColor> aInstanceAttributes)
       		: mSourceMesh(aSourceMesh)
		, mInstanceAttributes(std::move(aInstanceAttributes))
	{}

	// std::optional<RenderData> getRenderData(const RenderOptions &aOptions) const override {
	// 	auto it = mRenderInfos.find(aOptions.mode);
	// 	if (it == mRenderInfos.end()) {
	// 		return std::optional<RenderData>();
	// 	}
        //
        //
	// 	if (!it->second.shaderProgram || !it->second.geometry) {
	// 		return std::optional<RenderData>();
	// 	}
	// 	return std::optional<RenderData>(RenderData{
	// 			getModelMatrix(),
	// 			it->second.materialParams,
	// 			*(it->second.shaderProgram),
	// 			*(it->second.geometry)
	// 		});
	// }

	std::shared_ptr<AGeometry> getGeometry(GeometryFactory &aGeometryFactory, RenderStyle aRenderStyle) {

	 	return mSourceMesh->getGeometry(aGeometryFactory, aRenderStyle);
	}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		for (auto &mode : mRenderInfos) {
			mode.second.shaderProgram = aMaterialFactory.getShaderProgram(mode.second.materialParams.mMaterialName);
			getTextures(mode.second.materialParams.mParameterValues, aMaterialFactory);
			mode.second.geometry = getGeometry(aGeometryFactory, mode.second.materialParams.mRenderStyle);
		}
	}

protected:
	std::shared_ptr<MeshObject> mSourceMesh;
	std::vector<VertexColor> mInstanceAttributes;
};

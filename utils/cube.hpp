#pragma once

#include "mesh_object.hpp"


class Cube: public MeshObject {
public:
	Cube() {}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		for (auto &mode : mRenderInfos) {
			mode.second.shaderProgram = aMaterialFactory.getShaderProgram(mode.second.materialParams.mMaterialName);
			if (mode.second.materialParams.mRenderStyle == RenderStyle::Solid) {
				mode.second.geometry = aGeometryFactory.getCube();
			}
		}
	}
protected:
};


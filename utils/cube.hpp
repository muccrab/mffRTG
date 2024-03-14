#pragma once

#include "mesh_object.hpp"


class Cube: public MeshObject {
public:
	Cube() {}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		for (auto &mode : mRenderInfos) {
			mode.second.shaderProgram = aMaterialFactory.getShaderProgram(mode.second.materialParams.mMaterialName);
			getTextures(mode.second.materialParams.mParameterValues, aMaterialFactory);
			switch (mode.second.materialParams.mRenderStyle) {
			case RenderStyle::Solid:
				mode.second.geometry = aGeometryFactory.getCubeNormTex();
				break;
			case RenderStyle::Wireframe:
				mode.second.geometry = aGeometryFactory.getCubeOutline();
				break;
			}
		}
	}
protected:
};


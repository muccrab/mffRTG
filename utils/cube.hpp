#pragma once

#include "mesh_object.hpp"


class Cube: public MeshObject {
public:
	Cube() {}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		mShaderProgram = aMaterialFactory.getShaderProgram(mMaterialParams.mMaterialName);
		mGeometry = aGeometryFactory.getCube();
	}
protected:
};


#pragma once

#include <memory>
#include <vector>
#include <ranges>

#include "scene_object.hpp"
#include "cube.hpp"

#include "material_factory.hpp"
#include "geometry_factory.hpp"

class SimpleScene {
public:
	void addObject(std::shared_ptr<SceneObject> aNewObject) {
		mObjects.push_back(aNewObject);
	}

	auto getObjects() const {
		return mObjects | std::views::transform([](const auto &aPtr) { return *aPtr; });
	}
protected:
	std::vector<std::shared_ptr<SceneObject>> mObjects;
};


SimpleScene createCubeScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto cube = std::make_shared<Cube>();

	cube->setMaterial(
		MaterialParameters(
			"solid_color",
			{
				{ "color", glm::vec3(1.0f, 0.5f, 0.0f)}
			})
		);
	cube->prepareRenderData(aMaterialFactory, aGeometryFactory);

	scene.addObject(cube);
	return scene;
}

SimpleScene createSphereScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	return SimpleScene();
}

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
		return mObjects | std::views::transform([](const auto &aPtr) -> const SceneObject& { return *aPtr; });
	}
protected:
	std::vector<std::shared_ptr<SceneObject>> mObjects;
};


SimpleScene createCubeScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto cube = std::make_shared<Cube>();

	cube->setName("CUBE");
	cube->setMaterial(
		"solid",
		MaterialParameters(
			"solid_color",
			RenderStyle::Solid,
			{
				{ "color", glm::vec3(1.0f, 0.5f, 0.0f)}
			})
		);
	cube->prepareRenderData(aMaterialFactory, aGeometryFactory);

	// cube->setRotation(glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)));
	scene.addObject(cube);
	return scene;
}


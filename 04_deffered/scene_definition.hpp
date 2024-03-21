#pragma once

#include <memory>
#include <vector>
#include <ranges>

#include "scene_object.hpp"
#include "cube.hpp"

#include "material_factory.hpp"
#include "geometry_factory.hpp"
#include "simple_scene.hpp"

inline SimpleScene createCottageScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	// auto mesh = std::make_shared<LoadedMeshObject>("./geometry/cottage.obj");
	auto mesh = std::make_shared<Cube>();
	// auto mesh = std::make_shared<LoadedMeshObject>("./geometry/box.obj");

	mesh->setScale(glm::vec3(0.5));
	mesh->setName("MONKEY");
	// mesh->addMaterial(
	// 	"solid",
	// 	MaterialParameters(
	// 		"solid_color",
	// 		RenderStyle::Solid,
	// 		{
	// 			{"u_solidColor", glm::vec4(0,0.5,0.5,1)}
	// 		})
	// 	);
	mesh->addMaterial(
		"solid",
		MaterialParameters(
			"material_deffered",
			RenderStyle::Solid,
			{
				{ "u_diffuseTexture", TextureInfo("cottage/cottageDif.jpg") },
				{ "light.ambient", glm::vec3(0.3, 0.3, 0.3) },
				{ "light.diffuse", glm::vec3(0.6, 0.6, 0.6) },
				{ "light.position", glm::vec3(3.0, 5.0, 6.0) },
				{ "light.specular", glm::vec3(1.0, 1.0, 1.0) },
				{ "material.ambient", glm::vec3(0.1, 0.1, 0.1) },
				{ "material.diffuse", glm::vec3(1.0, 0.5, 0.31) },
				{ "material.shininess", 32.0f },
				{ "material.specular", glm::vec3(0.5, 0.5, 0.5) },
			}
			)
		);
	// mesh->addMaterial(
	// 	"wireframe",
	// 	MaterialParameters(
	// 		"solid_color",
	// 		RenderStyle::Wireframe,
	// 		{}
	// 		)
	// 	);
	mesh->prepareRenderData(aMaterialFactory, aGeometryFactory);

	scene.addObject(mesh);
	return scene;
}

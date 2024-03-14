#pragma once

#include <memory>
#include <vector>
#include <ranges>

#include "scene_object.hpp"
#include "cube.hpp"

#include "material_factory.hpp"
#include "geometry_factory.hpp"
#include "simple_scene.hpp"


inline SimpleScene createCubeScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto cube = std::make_shared<Cube>();

	cube->setName("CUBE");
	cube->addMaterial(
		"solid",
		MaterialParameters(
			"normal_to_color",
			RenderStyle::Solid,
			{}
			)
		);
	cube->addMaterial(
		"wireframe",
		MaterialParameters(
			"solid_color",
			RenderStyle::Wireframe,
			{}
			)
		);
	cube->prepareRenderData(aMaterialFactory, aGeometryFactory);

	scene.addObject(cube);
	return scene;
}

inline SimpleScene createTexturedCubeScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto cube = std::make_shared<Cube>();

	cube->setName("CUBE");
	cube->addMaterial(
		"solid",
		MaterialParameters(
			"texture_blend",
			RenderStyle::Solid,
			{
				{ "u_textureSampler1", TextureInfo("brick_wall/Brick_Wall_012_COLOR.jpg") },
				{ "u_textureSampler2", TextureInfo("pebbles/Pebbles_003_COLOR.jpg") }
			}
			)
		);
	cube->addMaterial(
		"wireframe",
		MaterialParameters(
			"solid_color",
			RenderStyle::Wireframe,
			{}
			)
		);
	cube->prepareRenderData(aMaterialFactory, aGeometryFactory);

	scene.addObject(cube);
	return scene;
}


inline SimpleScene createMonkeyScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto mesh = std::make_shared<LoadedMeshObject>("./geometry/monkey.obj");
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
			"phong",
			RenderStyle::Solid,
			{}
			)
		);
	mesh->addMaterial(
		"wireframe",
		MaterialParameters(
			"solid_color",
			RenderStyle::Wireframe,
			{}
			)
		);
	mesh->prepareRenderData(aMaterialFactory, aGeometryFactory);

	scene.addObject(mesh);
	return scene;
}

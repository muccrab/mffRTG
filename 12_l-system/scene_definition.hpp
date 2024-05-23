#pragma once

#include <memory>
#include <vector>
#include <ranges>

#include "vertex.hpp"
#include "scene_object.hpp"
#include "cube.hpp"
#include "l_system.hpp"

#include "material_factory.hpp"
#include "geometry_factory.hpp"
#include "simple_scene.hpp"

constexpr unsigned int DIFFUSE = 1;
constexpr unsigned int SPECULAR = 1 << 1;
constexpr unsigned int BUMP = 1 << 2;
constexpr unsigned int PARALLAX = 1 << 3;
constexpr unsigned int AMBIENT_OCC = 1 << 4;
constexpr unsigned int SHADOW = 1 << 5;
constexpr unsigned int DEBUG = 1 << 7;


inline SimpleScene createLSystemScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	{
		auto tree = std::make_shared<LSystem>();
		tree->loadRuleSet("./rules/tree_02.rule");
		tree->runGenerations(7);

		tree->setName("TREE1");
		tree->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		tree->addMaterial(
			"solid",
			MaterialParameters(
				"branches",
				RenderStyle::Solid,
				{
					{"u_solidColor", glm::vec4(0.3, 0.2, 0.0, 1)}
				}
				)
			);
		tree->addMaterial(
			"wireframe",
			MaterialParameters(
				"solid_color",
				RenderStyle::Wireframe,
				{}
				)
			);
		tree->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(tree);
	}
	return scene;
}


#pragma once

#include <memory>
#include <vector>
#include <ranges>
#include <chrono>

#include "vertex.hpp"
#include "scene_object.hpp"
#include "cube.hpp"
#include "instanced_cube.hpp"
#include "particle.hpp"
// #include "instanced_mesh_object.hpp"

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



inline SimpleScene createParticleScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto particle = std::make_shared<Particle>(100);
	//particle->setScale(glm::vec3(0.1, 0.1, 0.1)); so far pointless, I might get it working IDK...
	particle->addMaterial(
		"solid",
		MaterialParameters(
			"particles",
			RenderStyle::Solid,
			{
				{"u_instanceCount", 100},
				{ "u_particleTexture", TextureInfo("blob.mhd") },
				{"u_emmiterDimensions", glm::vec3(0.1, 0.1f, 0.1)},
				{"u_particleLifeSize", glm::vec2(0.05, 0.5)},
				{"u_particleSpeed", 0.1f },
				{"u_particleDirection", glm::vec3(0,1,0) },
				{"u_particleSpread", 45.f},
				{"u_particleLifeVelocity", glm::vec2(1, 2)},
				{"u_rotationSpeed", 0.f},
				{"u_initRotation", 45.f},
				{"u_startColor", glm::vec4(0.3, 0.3, 0.3, 1)},
				{"u_endColor", glm::vec4(1, 1, 1, 1)},
				{"u_windLifeSpeed", glm::vec2(0.5,1) },
				{"u_gravityLifeForce", glm::vec2(0,0.1f)},
				{ "u_normalSpread", 3.f }
			}
			)
		);
	particle->addMaterial(
		"wireframe",
		MaterialParameters(
			"solid_color",
			RenderStyle::Wireframe,
			{}
			)
		);
	particle->prepareRenderData(aMaterialFactory, aGeometryFactory);

	scene.addObject(particle);
	return scene;
}

inline SimpleScene createParticle2Scene(MaterialFactory& aMaterialFactory, GeometryFactory& aGeometryFactory) {
	SimpleScene scene;
	auto particle = std::make_shared<Particle>(500);
	//particle->setScale(glm::vec3(0.1, 0.1, 0.1)); so far pointless, I might get it working IDK...
	particle->addMaterial(
		"solid",
		MaterialParameters(
			"particles",
			RenderStyle::Solid,
			{
				{"u_instanceCount", 500},
				{ "u_particleTexture", TextureInfo("spark.mhd") },
				{"u_emmiterDimensions", glm::vec3(0.5, 0.f, 0.5)},
				{"u_particleLifeSize", glm::vec2(0.1, 0.05)},
				{"u_particleSpeed", 0.4f },
				{"u_particleDirection", glm::vec3(0,1,0) },
				{"u_particleSpread", 0.f},
				{"u_particleLifeVelocity", glm::vec2(5, 10)},
				{"u_rotationSpeed", 1.f},
				{"u_initRotation", 0.f},
				{"u_startColor", glm::vec4(0.7, 0.3, 0.3, 1)},
				{"u_endColor", glm::vec4(0.7, 0.5, 0.3, 1)},
				{"u_windLifeSpeed", glm::vec2(0,0.1) },
				{"u_gravityLifeForce", glm::vec2(0,0)},
				{ "u_normalSpread", 0.f }
			}
			)
	);
	particle->addMaterial(
		"wireframe",
		MaterialParameters(
			"solid_color",
			RenderStyle::Wireframe,
			{}
		)
	);
	particle->prepareRenderData(aMaterialFactory, aGeometryFactory);

	scene.addObject(particle);
	return scene;
}
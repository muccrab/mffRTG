#pragma once

#include <memory>
#include <vector>

#include "scene_object.hpp"

class SimpleScene {
public:
	void addObject(std::shared_ptr<SceneObject> aNewObject) {
		mObject.push_back(aNewObject);
	}
protected:
	std::vector<std::shared_ptr<SceneObject>> mObjects;
}

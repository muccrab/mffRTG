#pragma once

// Represents a line segment in 3D space with thickness at each end
struct LineSegment {
    glm::vec3 start;
    glm::vec3 end;
    float startThickness;
    float endThickness;
};

// Generates the L-system geometry (list of lines) from the generated string
std::vector<LineSegment> generateLSystemGeometry(const std::string& lSystemString, float angle, float stepLength, float initialThickness, float shrinkageFactor) {
    std::vector<LineSegment> lines;
    std::stack<std::pair<glm::mat4, float>> transformStack; // Pair of transformation matrix and thickness
    glm::mat4 currentTransform = glm::mat4(1.0f); // Identity matrix
    glm::vec3 currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = glm::vec3(0.0f, stepLength, 0.0f);
    float currentThickness = initialThickness;

    for (const char& command : lSystemString) {
        if (command == 'F') {
            glm::vec4 newEnd = currentTransform * glm::vec4(direction, 1.0f);
            glm::vec3 endPosition = glm::vec3(newEnd) + currentPosition;
            float endThickness = currentThickness * shrinkageFactor;
            lines.push_back({currentPosition, endPosition, currentThickness, endThickness});
            currentPosition = endPosition;
            currentThickness = endThickness;
        } else if (command == '+') {
            currentTransform = glm::rotate(currentTransform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)); // Turn right around Z-axis
        } else if (command == '-') {
            currentTransform = glm::rotate(currentTransform, glm::radians(-angle), glm::vec3(0.0f, 0.0f, 1.0f)); // Turn left around Z-axis
        } else if (command == '&') {
            currentTransform = glm::rotate(currentTransform, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch down around X-axis
        } else if (command == '^') {
            currentTransform = glm::rotate(currentTransform, glm::radians(-angle), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch up around X-axis
        } else if (command == '/') {
            currentTransform = glm::rotate(currentTransform, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)); // Roll right around Y-axis
        } else if (command == '\\') {
            currentTransform = glm::rotate(currentTransform, glm::radians(-angle), glm::vec3(0.0f, 1.0f, 0.0f)); // Roll left around Y-axis
        } else if (command == '[') {
            transformStack.push({currentTransform, currentThickness});
        } else if (command == ']') {
            currentPosition = glm::vec3(transformStack.top().first * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            currentTransform = transformStack.top().first;
            currentThickness = transformStack.top().second;
            transformStack.pop();
        }
    }

    return lines;
}


class InstancedCube: public MeshObject {
public:
	InstancedCube(std::vector<VertexColor> aInstanceAttributes)
		: mInstanceAttributes(std::move(aInstanceAttributes))

	{}

	virtual std::shared_ptr<AGeometry> getGeometry(GeometryFactory &aGeometryFactory, RenderStyle aRenderStyle) {
		return std::make_shared<OGLGeometry>(generateInstancedCubeBuffers(mInstanceAttributes));
		// switch (aRenderStyle) {
		// case RenderStyle::Solid:
		// 	return aGeometryFactory.getCubeNormTex();
		// case RenderStyle::Wireframe:
		// 	return aGeometryFactory.getCubeOutline();
		// }
		// return std::shared_ptr<AGeometry>();
	}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		for (auto &mode : mRenderInfos) {
			mode.second.shaderProgram = aMaterialFactory.getShaderProgram(mode.second.materialParams.mMaterialName);
			getTextures(mode.second.materialParams.mParameterValues, aMaterialFactory);
			mode.second.geometry = getGeometry(aGeometryFactory, mode.second.materialParams.mRenderStyle);
		}
	}
protected:
	std::vector<VertexColor> mInstanceAttributes;
};



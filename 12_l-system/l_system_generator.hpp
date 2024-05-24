#pragma once


// Represents a line segment in 3D space with thickness at each end
struct LineSegment {
    glm::vec3 start;
    float startThickness;
    glm::vec3 end;
    float endThickness;
};

struct State {
	glm::mat4 transformation = glm::mat4(1.0f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	float shorteningFactor = 1.0;
	float level = 0;
};

// Generates the L-system geometry (list of lines) from the generated string
inline std::vector<LineSegment> generateLSystemGeometry(
		const std::string& lSystemString,
		float angle,
		float stepLength,
		float shorteningFactor) {
	std::vector<LineSegment> lines;
	std::stack<State> transformStack;
	State currentState;
	glm::vec3 direction = glm::vec3(0.0f, stepLength, 0.0f);

	for (const char& command : lSystemString) {
		switch (command) {
		case 'F': {
				glm::vec4 newEnd = currentState.transformation * glm::vec4(currentState.shorteningFactor * direction, 1.0f);
				glm::vec3 endPosition = glm::vec3(newEnd) + currentState.position;
				// std::cout << "LINE " << glm::length(newEnd) << " : " << glm::to_string(currentState.position) << " - " << glm::to_string(endPosition) << "\n";
				// std::cout << "CURRENT_TRANSFORM "<< glm::to_string(currentState.transformation) << "\n";
				lines.push_back(LineSegment{
					currentState.position, currentState.level,
					endPosition, currentState.level + 1.0f});
				currentState.position = endPosition;
				currentState.shorteningFactor *= shorteningFactor;
				currentState.level = currentState.level + 1.0f;
				break;
			  }
		case 'I':
			currentState.level = currentState.level + 1.0f;
			break;
		case 'S':
			currentState.shorteningFactor *= shorteningFactor;
			break;
		case '+':
			currentState.transformation = glm::rotate(currentState.transformation, glm::radians(angle), glm::vec3(currentState.transformation[0][0], currentState.transformation[1][0], currentState.transformation[2][0])); // Turn right around local Z-axis
			break;
		case '-':
			currentState.transformation = glm::rotate(currentState.transformation, glm::radians(-angle), glm::vec3(currentState.transformation[0][0], currentState.transformation[1][0], currentState.transformation[2][0])); // Turn left around local Z-axis
			break;
		case '>':
			currentState.transformation = glm::rotate(currentState.transformation, glm::radians(angle), glm::vec3(currentState.transformation[0][1], currentState.transformation[1][1], currentState.transformation[2][1])); // Pitch down around local X-axis
			break;
		case '<':
			currentState.transformation = glm::rotate(currentState.transformation, glm::radians(-angle), glm::vec3(currentState.transformation[0][1], currentState.transformation[1][1], currentState.transformation[2][1])); // Pitch up around local X-axis
			break;
		case '/':
			currentState.transformation = glm::rotate(currentState.transformation, glm::radians(angle), glm::vec3(currentState.transformation[0][2], currentState.transformation[1][2], currentState.transformation[2][2])); // Roll right around local Y-axis
			break;
		case '\\':
			currentState.transformation = glm::rotate(currentState.transformation, glm::radians(-angle), glm::vec3(currentState.transformation[0][2], currentState.transformation[1][2], currentState.transformation[2][2])); // Roll left around local Y-axis
			break;
		// case '+':
		// 	currentState.transformation = glm::rotate(currentState.transformation, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)); // Turn right around Z-axis
		// 	break;
		// case '-':
		// 	currentState.transformation = glm::rotate(currentState.transformation, glm::radians(-angle), glm::vec3(0.0f, 0.0f, 1.0f)); // Turn left around Z-axis
		// 	break;
		// case '>':
		// 	currentState.transformation = glm::rotate(currentState.transformation, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch down around X-axis
		// 	break;
		// case '<':
		// 	currentState.transformation = glm::rotate(currentState.transformation, glm::radians(-angle), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch up around X-axis
		// 	break;
		// case '/':
		// 	currentState.transformation = glm::rotate(currentState.transformation, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Roll right around Y-axis
		// 	break;
		// case '\\':
		// 	currentState.transformation = glm::rotate(currentState.transformation, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Roll left around Y-axis
		// 	break;
		case '[':
			transformStack.push(currentState);
			break;
		case ']': {
				currentState = transformStack.top();
				transformStack.pop();
				break;
			}
		default:
			// Ignore unrecognized characters
			break;
		}
	}

	return lines;
}



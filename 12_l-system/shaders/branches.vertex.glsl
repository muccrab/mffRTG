#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in float aThickness;

out vec3 fragPos;
out float fragThickness;

void main()
{
    fragPos = aPos;
    fragThickness = aThickness;
}


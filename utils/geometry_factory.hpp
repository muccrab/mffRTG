#pragma once

class AGeometry {

};

class GeometryFactory {
public:

	virtual std::shared_ptr<AGeometry> getCube() = 0;

};


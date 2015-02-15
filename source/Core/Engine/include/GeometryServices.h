/*
 * GeometryServices.h
 *
 *  Created on: 14-Feb-2015
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_GEOMETRYSERVICES_H_
#define CORE_ENGINE_INCLUDE_GEOMETRYSERVICES_H_

namespace nextar {

class GeometryServices {
public:

	static Polygon CreateSphere(uint32 density, float radius,
			bool normalData = false, bool colorData = false, const Color& color = Color::Black);
	static Polygon CreateCone(uint32 density, float baseRadius,
			float topRadius,
			float height,
			bool normalData = false, bool colorData = false, const Color& topColor = Color::Black,
			const Color& bottomColor = Color::Black);
	static Polygon CreateCylinder(uint32 density, float radius,
			float height,
			bool normalData = false, bool colorData = false, const Color& topColor = Color::Black,
			const Color& bottomColor = Color::Black);
	static Polygon CreateCube(float dx, float dy, float dz,
			bool normalData = false, bool colorData = false, const Color& color = Color::Black);
	static Polygon CreateBox(float dx, float dy, float dz,
			bool colorData = false, const Color& color = Color::Black);

};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_GEOMETRYSERVICES_H_ */

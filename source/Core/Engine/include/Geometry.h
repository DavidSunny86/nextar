/*
 * Geometry.h
 *
 *  Created on: 14-Feb-2015
 *      Author: obhi
 */

#ifndef CORE_BASE_INCLUDE_MATHTYPES_POLYGON_H_
#define CORE_BASE_INCLUDE_MATHTYPES_POLYGON_H_

#include <NexEngine.h>
#include <NexMath.h>
#include <Color.h>

namespace nextar {

class _NexEngineAPI Geometry : public AllocGeneral {
public:
	enum Type {
		LINES,
		POINTS,
		TRIANGLES,
	};

	typedef vector<Vector3>::type PointList;
	typedef vector<Color>::type ColorList;
	typedef vector<Vector2>::type Point2DList;
	typedef vector<uint16>::type IndexList;

	inline Geometry() : type(TRIANGLES) {}
	inline Geometry(PointList&& v) : points(std::move(v)), type(TRIANGLES) {}
	inline Geometry(PointList&& v, Type y) : points(std::move(v)), type(y) {}
	inline Geometry(PointList&& v, IndexList&& t) : points(std::move(v)),
			topology(std::move(t)), type(TRIANGLES) {}
	inline Geometry(PointList&& v, PointList&& n, IndexList&& t) :
			points(std::move(v)), normals(std::move(n)),
			topology(std::move(t)), type(TRIANGLES) {}
	inline Geometry(PointList&& v, PointList&& n, ColorList&& c, IndexList&& t) :
				points(std::move(v)), normals(std::move(n)),
				colors(std::move(c)),
				topology(std::move(t)), type(TRIANGLES) {}
	inline Geometry(PointList&& v, PointList&& n, ColorList&& c) :
					points(std::move(v)), normals(std::move(n)),
					colors(std::move(c)),
					type(TRIANGLES) {}
	inline Geometry(PointList&& v, IndexList&& t, Type y) : points(std::move(v)),
				topology(std::move(t)), type(y) {}



	static Geometry CreateSphere(uint32 density, float radius,
		bool normalData = false, bool colorData = false, const Color& color = Color::Black);
	static Geometry CreateCone(uint32 density, float baseRadius,
		float topRadius,
		float height,
		bool normalData = false, bool colorData = false, const Color& topColor = Color::Black,
		const Color& bottomColor = Color::Black);
	static Geometry CreateCylinder(uint32 density, float radius,
		float height,
		bool normalData = false, bool colorData = false, const Color& topColor = Color::Black,
		const Color& bottomColor = Color::Black);
	static Geometry CreateCube(float dx, float dy, float dz,
		bool normalData = false, bool colorData = false, const Color& color = Color::Black);
	static Geometry CreateBox(float dx, float dy, float dz,
		bool colorData = false, const Color& color = Color::Black);


	Type type;
	IndexList topology;
	PointList points;
	PointList normals;
	Point2DList uv;
	ColorList colors;
};

} /* namespace nextar */

#endif /* CORE_BASE_INCLUDE_MATHTYPES_POLYGON_H_ */

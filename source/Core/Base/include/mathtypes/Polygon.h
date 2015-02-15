/*
 * Polygon.h
 *
 *  Created on: 14-Feb-2015
 *      Author: obhi
 */

#ifndef CORE_BASE_INCLUDE_MATHTYPES_POLYGON_H_
#define CORE_BASE_INCLUDE_MATHTYPES_POLYGON_H_

namespace nextar {

class _NexBaseAPI Polygon : public AllocGeneral {
public:
	enum Type {
		LINES,
		POINTS,
		TRIANGLES,
	};

	typedef vector<Vector3>::type PointList;
	typedef vector<Vector2>::type Point2DList;
	typedef vector<uint16>::type IndexList;

	inline Polygon() : type(TRIANGLES) {}
	inline Polygon(PointList&& v) : points(std::move(v)), type(TRIANGLES) {}
	inline Polygon(PointList&& v, Type y) : points(std::move(v)), type(y) {}
	inline Polygon(PointList&& v, IndexList&& t) : points(std::move(v)),
			topology(std::move(t)), type(TRIANGLES) {}
	inline Polygon(PointList&& v, PointList&& n, IndexList&& t) :
			points(std::move(v)), normals(std::move(n)),
			topology(std::move(t)), type(TRIANGLES) {}
	inline Polygon(PointList&& v, PointList&& n, PointList&& c, IndexList&& t) :
				points(std::move(v)), normals(std::move(n)),
				colors(std::move(c)),
				topology(std::move(t)), type(TRIANGLES) {}
	inline Polygon(PointList&& v, PointList&& n, PointList&& c) :
					points(std::move(v)), normals(std::move(n)),
					colors(std::move(c)),
					type(TRIANGLES) {}
	inline Polygon(PointList&& v, IndexList&& t, Type y) : points(std::move(v)),
				topology(std::move(t)), type(y) {}

	Type type;
	IndexList topology;
	PointList points;
	PointList normals;
	Point2DList uv;
	PointList colors;
};

} /* namespace nextar */

#endif /* CORE_BASE_INCLUDE_MATHTYPES_POLYGON_H_ */

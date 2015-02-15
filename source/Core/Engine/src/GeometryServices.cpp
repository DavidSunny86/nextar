/*
 * GeometryServices.cpp
 *
 *  Created on: 14-Feb-2015
 *      Author: obhi
 */

#include <GeometryServices.h>

namespace nextar {

Polygon GeometryServices::CreateSphere(uint32 density, float radius,
		bool normalData, bool colorData, const Color& color) {
	// @optimize Its probably better to use a single vertex buffer for all volumes
	uint32 width = density;
	uint32 height = density;
	uint32 nvec = (height-2)* width+2;
	uint32 ntri = (height-2)*(width-1)*2;
	uint32 vbsize = normalData ? nvec * 2 : nvec;
	uint32 ibsize = ntri * 3;

	Polygon::PointList points;
	Polygon::PointList normals;
	Polygon::PointList colors;
	Polygon::IndexList indices;
	points.reserve(vbsize);
	indices.reserve(ibsize);

	for(uint32 j=1; j<height-1; j++) {
		float theta = float(j)/(height-1) * Math::PI;
		float sinTheta, cosTheta;
		Math::SinCos(theta, sinTheta, cosTheta);
		for(uint32 i=0; i<width; i++ ) {
			float phi   = float(i)/(width-1 ) * Math::TWO_PI;
			float sinPhi, cosPhi;
			Math::SinCos(phi, sinPhi, cosPhi);
			Vector3 pos(radius*sinTheta * cosPhi,
					    		radius*cosTheta,
								-radius*sinTheta * sinPhi);
			points.push_back(pos);
		}
	}
	points.push_back(Vector3(0, radius, 0));
	points.push_back(Vector3(0,-radius, 0));

	if (normalData) {
		normals.reserve(points.size());
		for(auto &p : points) {
			normals.push_back(p);
			normals.back().Normalize();
		}
	}


	for(uint32 j=0; j<height-3; j++ ) {
	  for(uint32 i=0; i<width-1; i++ ) {
	    indices.push_back((uint16) (j  )*width + i);
	    indices.push_back((uint16) (j+1)*width + i+1);
	    indices.push_back((uint16) (j  )*width + i+1);
	    indices.push_back((uint16) (j  )*width + i  );
	    indices.push_back((uint16) (j+1)*width + i  );
	    indices.push_back((uint16) (j+1)*width + i+1);
	  }
	}
	for(uint32 i=0; i<width-1; i++ ) {
		indices.push_back((uint16) (height-2)*width);
		indices.push_back((uint16) i);
		indices.push_back((uint16) i+1);
		indices.push_back((uint16) (height-2)*width+1);
		indices.push_back((uint16) (height-3)*width + i+1);
		indices.push_back((uint16) (height-3)*width + i);
	}

	if (colorData) {
		colors.reserve(points.size());
		Vector3 c(color.red, color.green, color.blue);
		for(uint32 i = 0; i < points.size(); ++i)
			colors.push_back(c);
	}
	return Polygon(std::move(points), std::move(normals), std::move(colors), std::move(indices));
}

Polygon GeometryServices::CreateCone(uint32 density,
		float baseRadius, float topRadius, float height,
		bool normalData, bool colorData, const Color& topColor,
		const Color& bottomColor) {

	bool topCapped = (topRadius > 0) != 0;
	uint32 numVec = topCapped ? (density + 1) * 2 : 2 + density;
	uint32 vbsize = normalData ? numVec * 2 : numVec;
	uint32 ntri = topCapped ? 4*density : 2*density;
	if (topRadius <= 0.0f)
		topRadius = 0.0f;
	float heightCap = height * topRadius / baseRadius;
	uint32 ibsize = ntri*3;
	Polygon::PointList points;
	Polygon::PointList normals;
	Polygon::PointList colors;
	Polygon::IndexList indices;
	points.reserve(vbsize);
	indices.reserve(ibsize);

	float angleStepper = Math::TWO_PI / (density+1);


	points.push_back(Vector3(0, heightCap, 0));
	points.push_back(Vector3(0, 0, 0));

	float ratio = ((height - heightCap) / height) * baseRadius;
	float theta = 0;
	if (topCapped) {
		for(uint32 i = 0; i < density; ++i, theta += angleStepper) {

			Vector3 p(
					ratio * Math::Cos(theta),
					heightCap,
					ratio * Math::Sin(theta));
			points.push_back(p);

		}
	}
	ratio = baseRadius;
	theta = 0;
	for(uint32 i = 0; i < density; ++i, theta += angleStepper) {

		Vector3 p(
			ratio * Math::Cos(theta),
			0,
			ratio * Math::Sin(theta));
		points.push_back(p);
	}

	if (colorData) {
		Vector3 topCol(topColor.red, topColor.green, topColor.blue);
		Vector3 botCol(bottomColor.red, bottomColor.green, bottomColor.blue);
		colors.reserve(points.size());
		colors.push_back(topCol);
		colors.push_back(botCol);
		for(uint32 i = 0; i < density; ++i)
			colors.push_back(topCol);
		for(uint32 i = 0; i < density; ++i)
			colors.push_back(botCol);
	}

	if (normalData) {
		normals.push_back(Vector3(0, 1, 0));
		normals.push_back(Vector3(0,-1, 0));
		if (topCapped) {
			for(uint32 i = 0; i < density; ++i) {
				Vector3& p = points[i+2];
				Vector3 n(p.x, 0, p.z);
				n.Normalize();
				normals.push_back(n);
			}
		}

		for(uint32 i = 0; i < density; ++i) {
			Vector3& p = points[i+2];
			Vector3 n(p.x, 0, p.z);
			n.Normalize();
			normals.push_back(n);
		}
	}

	uint16 botcapOff = 0;
	for(uint16 i = 0; i < (uint16)density; ++i) {
		indices.push_back(0);
		indices.push_back(i+2);
		indices.push_back( ((i+1)%(density)) + 2 );
	}

	if (topCapped) {
		botcapOff = density;
	}

	for(uint16 i = 0; i < (uint16)density; ++i) {
		indices.push_back(1);
		indices.push_back( botcapOff + ((i+1)%(density)) + 2 );
		indices.push_back(botcapOff + i+2);
	}

	if (topCapped) {
		for(uint16 i = 0; i < (uint16)density; ++i) {
			uint16 i0 = i+2;
			uint16 i1 = (i+1)%(density) + 2;
			uint16 i2 = density+i0;
			uint16 i3 = density+i1;

			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i3);
			indices.push_back(i3);
			indices.push_back(i1);
			indices.push_back(i0);
		}
	}

	return Polygon(std::move(points), std::move(normals), std::move(colors), std::move(indices));
}

Polygon GeometryServices::CreateCylinder(
		uint32 density, float radius,
		float height, bool normalData,
		bool colorData, const Color& topColor,
		const Color& bottomColor) {
	return CreateCone(density, radius, radius, height, normalData,
			colorData, topColor, bottomColor);
}

Polygon GeometryServices::CreateCube(float dx, float dy, float dz,
		bool normalData, bool colorData, const Color& color) {
	Polygon::PointList positions = {
		Vector3(-1.0f,-1.0f,-1.0f), // triangle 1 : begin
		Vector3(-1.0f,-1.0f, 1.0f),
		Vector3(-1.0f, 1.0f, 1.0f), // triangle 1 : end
		Vector3(-1.0f,-1.0f,-1.0f),
		Vector3(-1.0f, 1.0f, 1.0f),
		Vector3(-1.0f, 1.0f,-1.0f),

		Vector3( 1.0f, 1.0f,-1.0f), // triangle 2 : begin
		Vector3(-1.0f,-1.0f,-1.0f),
		Vector3(-1.0f, 1.0f,-1.0f), // triangle 2 : end
		Vector3(1.0f, 1.0f,-1.0f),
		Vector3(1.0f,-1.0f,-1.0f),
		Vector3(-1.0f,-1.0f,-1.0f),

		Vector3(1.0f,-1.0f, 1.0f),
		Vector3(-1.0f,-1.0f,-1.0f),
		Vector3(1.0f,-1.0f,-1.0f),
		Vector3(1.0f,-1.0f, 1.0f ),
		Vector3(-1.0f,-1.0f, 1.0f),
		Vector3(-1.0f,-1.0f,-1.0f),

		Vector3(-1.0f, 1.0f, 1.0f),
		Vector3(-1.0f,-1.0f, 1.0f),
		Vector3(1.0f,-1.0f, 1.0f),
		Vector3( 1.0f, 1.0f, 1.0f ),
		Vector3(-1.0f, 1.0f, 1.0f),
		Vector3( 1.0f,-1.0f, 1.0f),

		Vector3(1.0f, 1.0f, 1.0f),
		Vector3(1.0f,-1.0f,-1.0f),
		Vector3(1.0f, 1.0f,-1.0f),
		Vector3(1.0f,-1.0f,-1.0f),
		Vector3(1.0f, 1.0f, 1.0f),
		Vector3(1.0f,-1.0f, 1.0f),

		Vector3(1.0f, 1.0f, 1.0f),
		Vector3(1.0f, 1.0f,-1.0f),
		Vector3(-1.0f, 1.0f,-1.0f),
		Vector3( 1.0f, 1.0f, 1.0f ),
		Vector3(-1.0f, 1.0f,-1.0f),
		Vector3(-1.0f, 1.0f, 1.0f)
	};

	for(uint32 i = 0; i < 36; ++i) {
		positions[i].x *= dx;
		positions[i].y *= dy;
		positions[i].z *= dz;
	}

	Polygon::PointList normals;
	if (normalData) {

		Vector3 normalV[] = {
				Vector3(-1.0f, 0.0f, 0.0f), // triangle 1 : begin
				Vector3( 0.0f, 0.0f,-1.0f), // triangle 2 : begin
				Vector3( 0.0f,-1.0f, 0.0f),
				Vector3( 0.0f, 0.0f, 1.0f),
				Vector3( 1.0f, 0.0f, 0.0f),
				Vector3( 0.0f, 0.0f, 0.0f)
		};

		normals.reserve(positions.size());
		for(uint32 i = 0; i < 36; ++i) {
			normals.push_back(normalV[i/6]);
		}
	}

	Polygon::PointList colors;
	if (colorData) {
		Vector3 c(color.red, color.green, color.blue);
		colors.reserve(positions.size());
		for(uint32 i = 0; i < 36; ++i) {
			colors.push_back(c);
		}
	}

	return Polygon(std::move(positions), std::move(normals), std::move(colors));
}

Polygon GeometryServices::CreateBox(float width, float height, float depth,
		bool colorData, const Color& color) {

	Polygon::PointList points = {
		Vector3(-width, -height, -depth ), // 0
		Vector3(width, -height, -depth  ), // 1
		Vector3(width,  height, -depth  ), // 2
		Vector3(-width,  height, -depth ), // 3
		Vector3(-width, -height,  depth ), // 4
		Vector3(width, -height,  depth  ), // 5
		Vector3(width,  height,  depth  ), // 6
		Vector3(-width,  height,  depth )// 7
	};

	/***
	 *    7```````/6
	 *   /|      / |
	 *  3-------2  |
	 *  | |_____|__|
	 *  | 4     | /5
	 *  |/      |/
	 *  0-------1
	 */
	Polygon::PointList colors;
	if (colorData) {
		Vector3 c(color.red, color.green, color.blue);
		for(uint32 i = 0; i < 8; ++i) {
			colors.push_back(c);
		}
	}

	Polygon::IndexList indices = {
			0,1,
			1,2,
			2,3,
			3,0,
			4,5,
			5,6,
			6,7,
			7,4,
			4,0,
			3,7,
			1,5,
			2,6
	};

	Polygon p;
	p.points = std::move(points);
	p.colors = std::move(colors);
	p.topology = std::move(indices);
	p.type = Polygon::LINES;

	return p;
}

} /* namespace nextar */


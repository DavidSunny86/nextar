#ifndef NEXTAR_COLOR_H
#define NEXTAR_COLOR_H

#include <Serializer.h>

namespace nextar {

/**
 * @brief	Color encapsulates red,green,blue and alpha floating point channels.
 *
 * @author	Abhishek Dey
 * @date	12/24/2010
 */
class _NexBaseAPI Color {
public:
	float red;
	float green;
	float blue;
	float alpha;

	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;

	inline Color() : red(0), green(0), blue(0), alpha(1) {
	}

	inline Color(float a) : alpha(a), red(a), green(a), blue(a) {
	}

	inline Color(float a, float r, float g, float b) : alpha(a),
	red(r), green(g), blue(b) {
	}

	inline float& operator [] (int i) {
		return AsFloatArray()[i];
	}

	float* AsFloatArray() {
		return &red;
	}

	const float* AsFloatArray() const {
		return &red;
	}

	friend inline OutputSerializer& operator << (OutputSerializer& ser, const Color& c) {
		ser << c.red << c.green << c.blue << c.alpha;
		return ser;
	}

	friend inline InputSerializer& operator >> (InputSerializer& ser, Color& c) {
		ser >> c.red >> c.green >> c.blue >> c.alpha;
		return ser;
	}

	inline uint32 ToRgba() const;
	inline uint32 ToArgb() const;
	inline uint32 ToAbgr() const;
};

// a 32bit color component
class _NexBaseAPI Color32 {
public:
	uint8 red;
	uint8 green;
	uint8 blue;
	uint8 alpha;

	static const Color32 Black;
	static const Color32 White;
	static const Color32 Red;
	static const Color32 Green;
	static const Color32 Blue;

	inline Color32() : red(0), green(0), blue(0), alpha(0xff) {
	}

	inline Color32(uint8 a, uint8 r, uint8 g, uint8 b) :
	red(r), green(g), blue(b), alpha(a) {
	}

	inline Color32(float a, float r, float g, float b) :
		red((uint8)(r*255.f)), green((uint8)(g*255.f)), blue((uint8)(b*255.f)), alpha((uint8)(a*255.f)) {
	}

	inline Color32(const Color& c) :
		red((uint8)(c.red*255.f)), green((uint8)(c.green*255.f)), blue((uint8)(c.blue*255.f)), alpha((uint8)(c.alpha*255.f)) {
	}

	inline
	uint32 ToRgba() const {
		return (red << 24) | (green << 16) | (blue << 8) | alpha;
	}

	inline
	uint32 ToArgb() const {
		return (alpha << 24) | (red << 16) | (green << 8) | blue;
	}

	inline
	uint32 ToBgra() const {
		return (blue << 24) | (green << 16) | (red << 8) | alpha;
	}

	inline
	uint32 ToAbgr() const {
		return (alpha << 24) | (blue << 16) | (green << 8) | red;
	}

	// todo Not very efficient
	friend inline OutputSerializer& operator << (OutputSerializer& ser, const Color32& c) {
		ser << c.red << c.green << c.blue << c.alpha;
		return ser;
	}

	friend inline InputSerializer& operator << (InputSerializer& ser, Color32& c) {
		ser >> c.red >> c.green >> c.blue >> c.alpha;
		return ser;
	}
};

inline 
uint32 Color::ToRgba() const {
	uint8 _red((uint8)(red*255.f)), _green((uint8)(green*255.f)), _blue((uint8)(blue*255.f)), _alpha((uint8)(alpha*255.f));
	return (_red << 24) | (_green << 16) | (_blue << 8) | _alpha;
}

inline
uint32 Color::ToArgb() const {
	uint8 _red((uint8)(red*255.f)), _green((uint8)(green*255.f)), _blue((uint8)(blue*255.f)), _alpha((uint8)(alpha*255.f));
	return (_alpha << 24) | (_red << 16) | (_green << 8) | _blue;
}

inline
uint32 Color::ToAbgr() const {
	uint8 _red((uint8)(red*255.f)), _green((uint8)(green*255.f)), _blue((uint8)(blue*255.f)), _alpha((uint8)(alpha*255.f));
	return (_alpha << 24) | (_blue << 16) | (_green << 8) | _red;
}

}
#endif //NEXTAR_COLOR_H
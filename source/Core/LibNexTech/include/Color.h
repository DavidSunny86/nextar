#ifndef NEXTAR_COLOR_H
#define NEXTAR_COLOR_H

namespace nextar {

	/**
	 * @brief	Color encapsulates red,green,blue and alpha floating point channels.
	 *
	 * @author	Abhishek Dey
	 * @date	12/24/2010
	 */
	struct Color {
		float red;
		float green;
		float blue;
		float alpha;

		static const Color Black;
		static const Color White;
		static const Color Red;

		inline Color() : red(0), green(0), blue(0), alpha(1) {
		}

		inline Color(float a, float r, float g, float b) : alpha(a),
		red(r), green(g), blue(b) {
		}

		const float* AsFloatArray() const {
			return &red;
		}
	};

// a 32bit color component
	struct Color32 {
		uint8 red;
		uint8 green;
		uint8 blue;
		uint8 alpha;

		inline Color32() : red(0), green(0), blue(0), alpha(0xff) {
		}

		inline Color32(uint8 a, uint8 r, uint8 g, uint8 b) :
				red(r), green(g), blue(b), alpha(a) {
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
	};
}
#endif //NEXTAR_COLOR_H

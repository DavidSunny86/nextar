/*
 * VideoMode.h
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */

#ifndef VIDEOMODE_H_
#define VIDEOMODE_H_

#include "NexBase.h"

namespace nextar {

	class _NexEngineExport VideoMode {
	public:

		static inline String ToString(const VideoMode& vm) {
			return vm._ToString();
		}

		inline  String _ToString() const {
			OutStringStream strm;
			strm << width << 'x' << height << '@' << refreshRate;
			return strm.str();
		}

		// write from string

		static inline VideoMode FromString(const String& what) {
			VideoMode vm;
			vm._FromString(what);
			return vm;
		}

		inline void _FromString(const String & what) {
			if (what == StringUtils::Null)
				return;
			InStringStream strm(what);
			char dot1, dot2;
			strm >> width >> dot1 >> height >> dot2 >> refreshRate;
		}

		inline VideoMode() :
				width(0), height(0), refreshRate(0) {
		}

		inline VideoMode(const String & str) {
			FromString(str);
		}

		inline VideoMode(uint16 _width, uint16 _height, uint16 _refresh_rate) :
				width(_width), height(_height), refreshRate(_refresh_rate) {
		}

		inline VideoMode(const VideoMode & other) :
				width(other.width), height(other.height), refreshRate(
						other.refreshRate) {
		}

		inline
		bool operator ==(const VideoMode & other) const {
			return width == other.width && height == other.height
					&& refreshRate == other.refreshRate;
		}

		inline
		bool operator<(const VideoMode & other) const {
			return width < other.width || height < other.height
					|| refreshRate < other.refreshRate;
		}

		uint16 width;
		uint16 height;
		uint16 refreshRate;
	};

	typedef vector<VideoMode>::type VideoModeList;
}

#endif /* VIDEOMODE_H_ */

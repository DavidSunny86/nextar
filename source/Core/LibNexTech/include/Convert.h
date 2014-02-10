#ifndef NEXTAR_NEXCONV_H
#define NEXTAR_NEXCONV_H


#include "NexSys.h"
#include "NexMath.h"
#include "MathTypes.h"
#include "Color.h"

namespace nextar {
//! Generic conversion routines.
	namespace Convert {
		_NexExport String ToString(uint32 num, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0));
		_NexExport String ToString(int32 num, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0));

		_NexInline String ToString(uint16 num, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0)) {
			return ToString((uint32) num, fill, flags);
		}

		_NexInline String ToString(int16 num, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0)) {
			return ToString((int32) num, fill, flags);
		}
		_NexExport String ToString(float num, uint16 prec = 6, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0));
		_NexExport String ToString(bool val, bool yesno = false);
		_NexExport String ToVersionString(VersionID);
		_NexExport String ToString(Vec4AF mat);
		_NexExport String ToString(Mat4x4F mat);
		_NexExport String ToString(const Vector3& mat);
		_NexExport String ToString(const Vector2& mat);
		_NexExport String ToString(const Color& mat);
		_NexExport String ToString(const Color32& mat);
		_NexExport int32 ToLong(const String&);
		_NexExport size_t ToULong(const String&);
		_NexExport float ToFloat(const String&);
		_NexExport bool ToBool(const String&);
		_NexExport VersionID ToVersion(const String&);
		_NexExport Vector4A ToVec4A(const String&);
		_NexExport Matrix4x4 ToMat4x4(const String&);
		_NexExport Vector3 ToVector3(const String&);
		_NexExport Vector2 ToVector2(const String&);
		_NexExport Color ToColor(const String&);
		_NexExport Color32 ToColor32(const String&);
	}
}

#endif //NEXTAR_NEXCONV_H

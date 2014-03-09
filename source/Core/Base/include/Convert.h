#ifndef NEXTAR_NEXCONV_H
#define NEXTAR_NEXCONV_H


#include <NexBase.h>
#include <NexMath.h>
#include <MathTypes.h>
#include <Color.h>

namespace nextar {
//! Generic conversion routines.
	namespace Convert {
		_NexBaseExport String ToString(uint32 num, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0));
		_NexBaseExport String ToString(int32 num, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0));

		_NexInline String ToString(uint16 num, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0)) {
			return ToString((uint32) num, fill, flags);
		}

		_NexInline String ToString(int16 num, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0)) {
			return ToString((int32) num, fill, flags);
		}
		_NexBaseExport String ToString(float num, uint16 prec = 6, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0));
		_NexBaseExport String ToString(bool val, bool yesno = false);
		_NexBaseExport String ToVersionString(VersionID);
		_NexBaseExport String ToString(Vec4AF mat);
		_NexBaseExport String ToString(Mat4x4F mat);
		_NexBaseExport String ToString(const Vector3& mat);
		_NexBaseExport String ToString(const Vector2& mat);
		_NexBaseExport String ToString(const Color& mat);
		_NexBaseExport String ToString(const Color32& mat);
		_NexBaseExport int32 ToLong(const String&);
		_NexBaseExport size_t ToULong(const String&);
		_NexBaseExport float ToFloat(const String&);
		_NexBaseExport bool ToBool(const String&);
		_NexBaseExport VersionID ToVersion(const String&);
		_NexBaseExport Vector4A ToVec4A(const String&);
		_NexBaseExport Matrix4x4 ToMat4x4(const String&);
		_NexBaseExport Vector3 ToVector3(const String&);
		_NexBaseExport Vector2 ToVector2(const String&);
		_NexBaseExport Color ToColor(const String&);
		_NexBaseExport Color32 ToColor32(const String&);
	}
}

#endif //NEXTAR_NEXCONV_H

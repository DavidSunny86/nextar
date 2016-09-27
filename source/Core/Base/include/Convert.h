#ifndef NEXTAR_NEXCONV_H
#define NEXTAR_NEXCONV_H

#include <NexBase.h>
#include <NexMath.h>
#include <MathTypes.h>
#include <Color.h>
#include <ApplicationContextType.h>

namespace nextar {
//! Generic conversion routines.
namespace Convert {
_NexBaseAPI String ToString(uint32 num, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0),
		std::ios::fmtflags mask = std::ios::fmtflags(0));
_NexBaseAPI String ToString(int32 num, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0),
		std::ios::fmtflags mask = std::ios::fmtflags(0));

_NexInline String ToString(uint16 num, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0),
		std::ios::fmtflags mask = std::ios::fmtflags(0)) {
	return ToString((uint32) num, fill, flags, mask);
}

_NexInline String ToString(int16 num, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0),
		std::ios::fmtflags mask = std::ios::fmtflags(0)) {
	return ToString((int32) num, fill, flags, mask);
}
_NexBaseAPI String ToString(float num, uint16 prec = 6, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0),
		std::ios::fmtflags mask = std::ios::fmtflags(0));
_NexBaseAPI String ToString(bool val, bool yesno = false);

inline String ToString(const ApplicationContextType& t) {
	return t.ToString();
}

_NexBaseAPI String ToVersionString(VersionID);
_NexBaseAPI String ToString(Vec4AF mat);
_NexBaseAPI String ToString(Mat4x4F mat);
_NexBaseAPI String ToString(Mat3x4F mat);
_NexBaseAPI String ToString(const Vector3& mat);
_NexBaseAPI String ToString(const Vector2& mat);
_NexBaseAPI String ToString(const nextar::IVector2& v);
_NexBaseAPI String ToString(const nextar::IVector3& v);
_NexBaseAPI String ToString(const nextar::IVector4& v);
_NexBaseAPI String ToString(const Color& mat);
_NexBaseAPI String ToString(const Color32& mat);
_NexBaseAPI int32 ToLong(const String&);
_NexBaseAPI uint32 ToULong(const String&);
_NexBaseAPI float ToFloat(const String&);
_NexBaseAPI bool ToBool(const String&);
_NexBaseAPI VersionID ToVersion(const String&);
_NexBaseAPI Matrix4x4 ToMat4x4(const String&);
_NexBaseAPI Matrix3x4 ToMat3x4(const String&);
_NexBaseAPI Vector4 ToVector4(const String&);
_NexBaseAPI Vector3 ToVector3(const String&);
_NexBaseAPI Vector2 ToVector2(const String&);
_NexBaseAPI IVector2 ToIVector2(const String&);
_NexBaseAPI IVector3 ToIVector3(const String&);
_NexBaseAPI IVector4 ToIVector4(const String&);
_NexBaseAPI Color ToColor(const String&);
_NexBaseAPI Color32 ToColor32(const String&);

inline String ToString(const StringID& s) {
	return Convert::ToString((uint32)s._value);
}

inline ApplicationContextType ToApplicationContext(const String& t) {
	ApplicationContextType ret;
	ret.FromString(t);
	return ret;
}

}
}

#endif //NEXTAR_NEXCONV_H
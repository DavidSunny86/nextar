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
_NexBaseAPI String ToString(Vec4::pref mat);
_NexBaseAPI String ToString(Mat4::pref mat);
_NexBaseAPI String ToString(Mat3::pref mat);
_NexBaseAPI String ToString(const Vec3::type& mat);
_NexBaseAPI String ToString(const Vec2::type& mat);
_NexBaseAPI String ToString(const IVec2::type& v);
_NexBaseAPI String ToString(const IVec3::type& v);
_NexBaseAPI String ToString(const IVec4::type& v);
_NexBaseAPI String ToString(const Color& mat);
_NexBaseAPI String ToString(const Color32& mat);
_NexBaseAPI int32 ToLong(const String&);
_NexBaseAPI uint32 ToULong(const String&);
_NexBaseAPI float ToFloat(const String&);
_NexBaseAPI bool ToBool(const String&);
_NexBaseAPI VersionID ToVersion(const String&);
_NexBaseAPI Mat4::type ToMat4x4(const String&);
_NexBaseAPI Mat3::type ToMat3x4(const String&);
_NexBaseAPI Vec4::type ToVector4(const String&);
_NexBaseAPI Vec3::type ToVector3(const String&);
_NexBaseAPI Vec2::type ToVector2(const String&);
_NexBaseAPI IVec2::type ToIVector2(const String&);
_NexBaseAPI IVec3::type ToIVector3(const String&);
_NexBaseAPI IVec4::type ToIVector4(const String&);
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

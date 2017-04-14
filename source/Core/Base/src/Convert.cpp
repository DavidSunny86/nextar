#include <BaseHeaders.h>
#include <Convert.h>

namespace nextar {
namespace Convert {

_NexBaseAPI String ToString(uint32 num, char fill, std::ios::fmtflags flags,
		std::ios::fmtflags mask) {
	OutStringStream buff;
	buff.fill(fill);
	if (flags) {
		if (mask)
			buff.setf(flags, mask);
		else
			buff.setf(flags);
	}
	buff << num;
	return buff.str();
}

_NexBaseAPI String ToString(int32 num, char fill, std::ios::fmtflags flags,
		std::ios::fmtflags mask) {
	OutStringStream buff;
	buff.fill(fill);
	if (flags) {
		if (mask)
			buff.setf(flags, mask);
		else
			buff.setf(flags);
	}
	buff << num;
	return buff.str();
}

_NexBaseAPI String ToString(float num, uint16 prec, char fill,
		std::ios::fmtflags flags,
		std::ios::fmtflags mask) {
	OutStringStream buff;
	buff.precision(prec);
	buff.fill(fill);
	if (flags) {
		if (mask)
			buff.setf(flags, mask);
		else
			buff.setf(flags);
	}
	buff << num;
	return buff.str();
}

_NexBaseAPI String ToString(bool val, bool yesno) {
	if (yesno)
		return val ? "yes" : "no";
	else
		return val ? "true" : "false";
}

_NexBaseAPI String ToVersionString(VersionID v) {
	OutStringStream buff;
	buff << NEX_GET_VER_MAJOR(v) << '.' << NEX_GET_VER_MINOR(v) << '.'
			<< NEX_GET_VER_MINREV(v);
	return buff.str();
}

_NexBaseAPI int32 ToLong(const String& str) {
	InStringStream strm(str);
	int32 ret = -1;
	strm >> ret;
	return ret;
}

_NexBaseAPI uint32 ToULong(const String& str) {
	InStringStream strm(str);
	uint32 ret = -1;
	strm >> ret;
	return ret;
}

_NexBaseAPI float ToFloat(const String& str) {
	InStringStream strm(str);
	float ret = 0;
	strm >> ret;
	return ret;
}

_NexBaseAPI bool ToBool(const String& str) {
	if (!StringUtils::NoCaseCompare(str.c_str(), "yes")
			|| !StringUtils::NoCaseCompare(str.c_str(), "true"))
		return true;
	return false;
}

_NexBaseAPI VersionID ToVersion(const String& ver) {
	InStringStream strm(ver);
	char dot1, dot2;
	uint32 maj, min, rev;
	strm >> maj >> dot1 >> min >> dot2 >> rev;
	if (dot1 == '.' && dot2 == '.')
		return NEX_MAKE_VERSION(maj, min, rev);
	else
		return static_cast<uint32>(Constants::INVALID_VERSION_ID);
}

_NexBaseAPI String ToString(nextar::Vec4::pref v) {
	OutStringStream ret;
	ret << Vec4AGetX(v) << ' ' << Vec4AGetY(v) << ' ' << Vec4AGetZ(v) << ' '
			<< Vec4AGetW(v);
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::Vec3::type& v) {
	OutStringStream ret;
	ret << v.x << ' ' << v.y << ' ' << v.z;
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::Vec2::type& v) {
	OutStringStream ret;
	ret << v.x << ' ' << v.y;
	return ret.str();
}

_NexBaseAPI String ToString(const IVec2::type& v) {
	OutStringStream ret;
	ret << v[0] << ' ' << v[1];
	return ret.str();
}

_NexBaseAPI String ToString(const IVec3::type& v) {
	OutStringStream ret;
	ret << v[0] << ' ' << v[1] << ' ' << v[2];
	return ret.str();
}

_NexBaseAPI String ToString(const IVec4::type& v) {
	OutStringStream ret;
	ret << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3];
	return ret.str();
}

_NexBaseAPI String ToString(nextar::Mat4::pref mat) {
	OutStringStream ret;
	ret << ' ';
	for (int32 i = 0; i < 4; ++i)
		ret << ToString(Mat4x4Row(mat, i)) << ' ';
	return ret.str();
}

_NexBaseAPI String ToString(nextar::Mat3::pref mat) {
	OutStringStream ret;
	ret << ' ';
	for (int32 i = 0; i < 3; ++i)
		ret << ToString(Mat3x4Row(mat, i)) << ' ';
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::Color& mat) {
	OutStringStream ret;
	ret << mat.red << ' ' << mat.green << ' ' << mat.blue << ' ' << mat.alpha;
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::Color32& mat) {
	OutStringStream ret;
	ret << (uint32) mat.red << ' ' << (uint32) mat.green << ' '
			<< (uint32) mat.blue << ' ' << (uint32) mat.alpha;
	return ret.str();
}

_NexBaseAPI nextar::Vec4::type ToVector4(const String& inp) {
	InStringStream ret(inp);

	float x, y, z, w;
	ret >> x >> y >> z >> w;
	return Vec4::type(x, y, z, w);
}

_NexBaseAPI nextar::Vec3::type ToVector3(const String& inp) {
	InStringStream ret(inp);
	Vec3::type v;
	ret >> v.x >> v.y >> v.z;
	return v;
}

_NexBaseAPI nextar::Vec2::type ToVector2(const String& inp) {
	InStringStream ret(inp);
	Vec2::type v;
	ret >> v.x >> v.y;
	return v;
}

_NexBaseAPI IVec2::type ToIVector2(const String& inp) {
	InStringStream ret(inp);
	IVec2::type v;
	ret >> v[0] >> v[1];
	return v;
}

_NexBaseAPI IVec3::type ToIVector3(const String& inp) {
	InStringStream ret(inp);
	IVec3::type v;
	ret >> v[0] >> v[1] >> v[2];
	return v;
}

_NexBaseAPI IVec4::type ToIVector4(const String& inp) {
	InStringStream ret(inp);
	IVec4::type v;
	ret >> v[0] >> v[1] >> v[2] >> v[4];
	return v;
}

_NexBaseAPI nextar::Mat4::type ToMat4x4(const String& inp) {
	nextar::Mat4::type ret;
	InStringStream stream(inp);
	float x, y, z, w;
	if (inp.length() >= 25) {
		for (int32 i = 0; i < 4; ++i) {
			stream >> x >> y >> z >> w;
			Mat4x4Row(ret,i)= Vec4ASet(x, y, z, w);
		}
	}
	return ret;
}

_NexBaseAPI nextar::Mat3::type ToMat3x4(const String& inp) {
	nextar::Mat3::type ret;
	InStringStream stream(inp);
	float x, y, z, w;
	if (inp.length() >= 25) {
		for (int32 i = 0; i < 3; ++i) {
			stream >> x >> y >> z >> w;
			Mat3x4Row(ret,i)= Vec4ASet(x, y, z, w);
		}
	}
	return ret;
}

_NexBaseAPI nextar::Color ToColor(const String& inp) {
	InStringStream ret(inp);
	float x, y, z, w;
	ret >> x >> y >> z >> w;
	return Color(x, y, z, w);
}

_NexBaseAPI nextar::Color32 ToColor32(const String& inp) {
	InStringStream ret(inp);
	uint32 x, y, z, w;
	ret >> x >> y >> z >> w;
	return Color32((uint8) x, (uint8) y, (uint8) z, (uint8) w);
}
}
}


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

_NexBaseAPI String ToString(nextar::Vec4AF v) {
	OutStringStream ret;
	ret << Vec4AGetX(v) << ' ' << Vec4AGetY(v) << ' ' << Vec4AGetZ(v) << ' '
			<< Vec4AGetW(v);
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::Vector3& v) {
	OutStringStream ret;
	ret << v.x << ' ' << v.y << ' ' << v.z;
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::Vector2& v) {
	OutStringStream ret;
	ret << v.x << ' ' << v.y;
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::IVector2& v) {
	OutStringStream ret;
	ret << v[0] << ' ' << v[1];
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::IVector3& v) {
	OutStringStream ret;
	ret << v[0] << ' ' << v[1] << ' ' << v[2];
	return ret.str();
}

_NexBaseAPI String ToString(const nextar::IVector4& v) {
	OutStringStream ret;
	ret << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3];
	return ret.str();
}

_NexBaseAPI String ToString(nextar::Mat4x4F mat) {
	OutStringStream ret;
	ret << ' ';
	for (int32 i = 0; i < 4; ++i)
		ret << ToString(Mat4x4Row(mat, i)) << ' ';
	return ret.str();
}

_NexBaseAPI String ToString(nextar::Mat3x4F mat) {
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

_NexBaseAPI nextar::Vector4 ToVector4(const String& inp) {
	InStringStream ret(inp);

	float x, y, z, w;
	ret >> x >> y >> z >> w;
	return Vector4(x, y, z, w);
}

_NexBaseAPI nextar::Vector3 ToVector3(const String& inp) {
	InStringStream ret(inp);
	Vector3 v;
	ret >> v.x >> v.y >> v.z;
	return v;
}

_NexBaseAPI nextar::Vector2 ToVector2(const String& inp) {
	InStringStream ret(inp);
	Vector2 v;
	ret >> v.x >> v.y;
	return v;
}

_NexBaseAPI nextar::IVector2 ToIVector2(const String& inp) {
	InStringStream ret(inp);
	IVector2 v;
	ret >> v[0] >> v[1];
	return v;
}

_NexBaseAPI nextar::IVector3 ToIVector3(const String& inp) {
	InStringStream ret(inp);
	IVector3 v;
	ret >> v[0] >> v[1] >> v[2];
	return v;
}

_NexBaseAPI nextar::IVector4 ToIVector4(const String& inp) {
	InStringStream ret(inp);
	IVector4 v;
	ret >> v[0] >> v[1] >> v[2] >> v[4];
	return v;
}

_NexBaseAPI nextar::Matrix4x4 ToMat4x4(const String& inp) {
	nextar::Matrix4x4 ret;
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

_NexBaseAPI nextar::Matrix3x4 ToMat3x4(const String& inp) {
	nextar::Matrix3x4 ret;
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


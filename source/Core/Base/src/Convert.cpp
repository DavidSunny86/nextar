#include <BaseHeaders.h>
#include <Convert.h>

namespace nextar {
	namespace Convert {

		_NexBaseExport String ToString(uint32 num, char fill,
				std::ios::fmtflags flags) {
			OutStringStream buff;
			buff.fill(fill);
			if (flags)
				buff.setf(flags);
			buff << num;
			return buff.str();
		}

		_NexBaseExport String ToString(int32 num, char fill,
				std::ios::fmtflags flags) {
			OutStringStream buff;
			buff.fill(fill);
			if (flags)
				buff.setf(flags);
			buff << num;
			return buff.str();
		}

		_NexBaseExport String ToString(float num, uint16 prec, char fill,
				std::ios::fmtflags flags) {
			OutStringStream buff;
			buff.precision(prec);
			buff.fill(fill);
			if (flags)
				buff.setf(flags);
			buff << num;
			return buff.str();
		}

		_NexBaseExport String ToString(bool val, bool yesno) {
			if (yesno)
				return val ? "yes" : "no";
			else
				return val ? "true" : "false";
		}

		_NexBaseExport String ToVersionString(VersionID v) {
			OutStringStream buff;
			buff << NEX_GET_VER_MAJOR(v) << '.' << NEX_GET_VER_MINOR(v) << '.'
					<< NEX_GET_VER_MINREV(v);
			return buff.str();
		}

		_NexBaseExport int32 ToLong(const String& str) {
			InStringStream strm(str);
			int32 ret = -1;
			strm >> ret;
			return ret;
		}

		_NexBaseExport size_t ToULong(const String& str) {
			InStringStream strm(str);
			size_t ret = -1;
			strm >> ret;
			return ret;
		}

		_NexBaseExport float ToFloat(const String& str) {
			InStringStream strm(str);
			float ret = 0;
			strm >> ret;
			return ret;
		}

		_NexBaseExport bool ToBool(const String& str) {
			if (!StringUtils::NoCaseCompare(str.c_str(), "yes")
					|| !StringUtils::NoCaseCompare(str.c_str(), "true"))
				return true;
			return false;
		}

		_NexBaseExport VersionID ToVersion(const String& ver) {
			InStringStream strm(ver);
			char dot1, dot2;
			uint32 maj, min, rev;
			strm >> maj >> dot1 >> min >> dot2 >> rev;
			if (dot1 == '.' && dot2 == '.')
				return NEX_MAKE_VERSION(maj, min, rev) ;
			else
				return CONST_INVALID_VERSION_ID;
		}

		_NexBaseExport String ToString(nextar::Vec4AF v) {
			OutStringStream ret;
			ret << Vec4AGetX(v) << ' ' << Vec4AGetY(v) << ' ' << Vec4AGetZ(v)
					<< ' ' << Vec4AGetW(v);
			return ret.str();
		}

		_NexBaseExport String ToString(const nextar::Vector3& v) {
			OutStringStream ret;
			ret << v.x << ' ' << v.y << ' ' << v.z;
			return ret.str();
		}

		_NexBaseExport String ToString(const nextar::Vector2& v) {
			OutStringStream ret;
			ret << v.x << ' ' << v.y;
			return ret.str();
		}

		_NexBaseExport String ToString(nextar::Mat4x4F mat) {
			OutStringStream ret;
			ret << ' ';
			for (int32 i = 0; i < 4; ++i)
				ret << ToString(Mat4x4Row(mat,i)) << ' ';
			return ret.str();
		}

		_NexBaseExport String ToString(const nextar::Color& mat) {
			OutStringStream ret;
			ret << mat.red << ' ' << mat.green << ' ' << mat.blue << ' ' << mat.alpha;
			return ret.str();
		}

		_NexBaseExport String ToString(const nextar::Color32& mat) {
			OutStringStream ret;
			ret << (uint32)mat.red << ' ' << (uint32)mat.green << ' ' << (uint32)mat.blue << ' ' << (uint32)mat.alpha;
			return ret.str();
		}

		_NexBaseExport nextar::Vector4A ToVec4A(const String& inp) {
			InStringStream ret(inp);

			float x, y, z, w;
			ret >> x >> y >> z >> w;
			return Vec4ASet(x, y, z, w);
		}

		_NexBaseExport nextar::Vector3 ToVector3(const String& inp) {
			InStringStream ret(inp);
			Vector3 v;
			ret >> v.x >> v.y >> v.z;
			return v;
		}

		_NexBaseExport nextar::Vector2 ToVector2(const String& inp) {
			InStringStream ret(inp);
			Vector2 v;
			ret >> v.x >> v.y;
			return v;
		}

		_NexBaseExport nextar::Matrix4x4 ToMat4x4(const String& inp) {
			nextar::Matrix4x4 ret;
			InStringStream stream(inp);
			float x, y, z, w;
			if (inp.length() >= 25) {
				for (int32 i = 0; i < 4; ++i) {
					stream >> x >> y >> z >> w;
					Mat4x4Row(ret,i) = Vec4ASet(x, y, z, w);
				}
			}
			return ret;
		}

		_NexBaseExport nextar::Color ToColor(const String& inp) {
			InStringStream ret(inp);
			float x, y, z, w;
			ret >> x >> y >> z >> w;
			return Color(x, y, z, w);
		}

		_NexBaseExport nextar::Color32 ToColor32(const String& inp) {
			InStringStream ret(inp);
			uint32 x, y, z, w;
			ret >> x >> y >> z >> w;
			return Color32((uint8)x, (uint8)y, (uint8)z, (uint8)w);
		}
	}
}


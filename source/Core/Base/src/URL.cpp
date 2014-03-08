#include "NexHeaders.h"
#include "URL.h"

namespace nextar {

//------------------------------------------------------------
	NameValueMap URL::_macroTable;

	URL::URL() {
	}

	URL::URL(const String& _path) {
		_Parse(_path);
	}

	URL::URL(const UniString& _path) {
		_Parse(StringUtils::ToUtf8(_path));
	}

	URL::URL(const URL& _url) :
			archiveName(_url.archiveName), relativePath(_url.relativePath) {
	}

	URL::URL(uint32 _url_type, const String& _url_archiveName,
			const String& _url_relativePath) :
			archiveName(_url_archiveName), relativePath(_url_relativePath) {
	}

	void URL::_Parse(const String& descriptor) {
		NEX_ASSERT(descriptor.length());
		size_t offset = 0;
		// Do we have a mount point?
		if (descriptor[0] == '{') {
			size_t arEn = descriptor.find_first_of('}');
			if (arEn != String::npos) {
				archiveName = descriptor.substr(1, arEn - 1);
				offset = arEn + 1;
			} else {
				Warn("malformed path archive name: " + descriptor);
				return;
			}
		}

		if (descriptor[offset] == '/')
			offset++;
		relativePath = _Resolve(descriptor, offset);
	}

	String URL::ToString() const {
		String returnStr;
		if (archiveName.length() > 0)
			returnStr = "{" + archiveName + "}/";
		returnStr += relativePath;
		return returnStr;
	}

	String URL::_Resolve(const String& path, size_t offset) {
		String ret;
		size_t lastPos = offset;
		size_t macroStart;
		while ((macroStart = path.find_first_of('$', lastPos)) != String::npos) {
			macroStart++;
			if (macroStart < path.length() && path[macroStart] == '{') {
				size_t macroEnd = path.find_first_of('}', macroStart);
				if (macroEnd != String::npos) {
					ret += path.substr(lastPos, (macroStart - 1) - lastPos);
					// find the macro
					macroStart++;
					String macroName = path.substr(macroStart,
							macroEnd - macroStart);
					NameValueMap::iterator it = _macroTable.find(macroName);
					if (it != _macroTable.end()) {
						ret += (*it).second;
					}
					lastPos = macroEnd + 1;
				}
			}
		}
		if (lastPos != String::npos)
			ret += path.substr(lastPos);
		return ret;
	}

	bool URL::operator==(const URL& other) const {
		return archiveName == other.archiveName
				&& relativePath == other.relativePath;
	}

	bool URL::IsReservedPath(const char* path) {
		return (path[0] == '.'
				&& (path[1] == 0 || (path[1] == '.' && path[2] == 0))) != 0;
	}

	String URL::GetAppendedPath(const String& basePath,
			const String& fileName) {
		if (!basePath.length())
			return fileName;
		else if (!fileName.length())
			return basePath;
		else
			return basePath + '/' + fileName;
	}

	String URL::GetExtension() const {
		String::size_type p = relativePath.find_last_of('.');
		if (p == String::npos)
			return StringUtils::Null;
		return relativePath.substr(p+1);

	}
}


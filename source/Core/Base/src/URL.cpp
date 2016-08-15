#include <BaseHeaders.h>
#include <URL.h>

namespace nextar {

//------------------------------------------------------------
NameValueMap URL::_macroTable;
const URL URL::Invalid;

URL::URL() {
}

URL::URL(const String& objectName, const String& fileExt, const String& possibleLocations) {
	_Determine(objectName, fileExt, possibleLocations);
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

URL::URL(URL&& _url) :
		archiveName(std::move(_url.archiveName)),
		relativePath(std::move(_url.relativePath)) {
}

URL::URL(const String& _url_archiveName,
		const String& _url_relativePath) :
		archiveName(_url_archiveName), relativePath(_url_relativePath) {
}

const URL& URL::operator =(const UniString& _path) {
	_Parse(StringUtils::ToUtf8(_path));
	return *this;
}

const URL& URL::operator =(const String& _path) {
	_Parse(_path);
	return *this;
}

const URL& URL::operator =(const URL& _path) {
	archiveName = _path.archiveName;
	relativePath = _path.relativePath;
	return *this;
}

const URL& URL::operator =(URL&& _path) {
	archiveName = std::move(_path.archiveName);
	relativePath = std::move(_path.relativePath);
	return *this;
}

void URL::_Parse(const String& descriptor) {
	NEX_ASSERT(descriptor.length());
	
	// Do we have a mount point?
	size_t offset = _DetermineMountPoint(descriptor, archiveName);

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
		} else {
			lastPos = macroStart;
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
	return (path[0] == '.' && (path[1] == 0 || (path[1] == '.' && path[2] == 0)))
			!= 0;
}

String URL::GetAppendedPath(const String& basePath, const String& fileName) {
	if (!basePath.length())
		return fileName;
	else if (!fileName.length())
		return basePath;
	else {
		if (basePath.back() != '/' && fileName.front() != '/')
			return basePath + '/' + fileName;
		else
			return basePath + fileName;
	}
}

String URL::GetExtension() const {
	String::size_type p = relativePath.find_last_of('.');
	if (p == String::npos)
		return StringUtils::Null;
	return relativePath.substr(p + 1);

}

String URL::GetComputedName() const {
	String::size_type end = relativePath.find_last_of('.');
	String::size_type start = relativePath.find_last_of('/');
	// "/Sn.ext"
	// becomes Sn
	if (start == String::npos)
		start = 0;
	else
		++start;
	return relativePath.substr(start, end - start);
}

String URL::GetComputedFilePath() const {
	String ret = ToString();
	String::size_type end = ret.find_last_of('/');
	if (end == String::npos) {
		ret += '/';
		return ret;
	} else {
		return ret.substr(0, end + 1);
	}
}

size_t URL::_DetermineMountPoint(const String& descriptor, String& out) {
	size_t offset = 0;
	// Do we have a mount point?
	if (descriptor[0] == '{') {
		size_t arEn = descriptor.find_first_of('}');
		if (arEn != String::npos) {
			out = descriptor.substr(1, arEn - 1);
			offset = arEn + 1;
		} else {
			return 0;
		}
	}
	return offset;
}

void URL::_Determine(const String& objectName, const String& fileExt, const String& possibleLocations) {
	
	String relativePathSource = objectName;
	size_t offset = _DetermineMountPoint(relativePathSource, archiveName);
	if (!offset) {
		relativePathSource = GetAppendedPath(possibleLocations, objectName);
		offset = _DetermineMountPoint(relativePathSource, archiveName);
	}

	if (offset < relativePathSource.length() &&
		relativePathSource[offset] == '/')
		offset++;
	
	relativePath = _Resolve(relativePathSource, offset);

	if (fileExt.length() > 0 && !StringUtils::EndsWith(relativePath, fileExt)) {
		if (fileExt[0] != '.')
			relativePath += '.';
		relativePath += fileExt;
	}
}

_NexBaseAPI OutputSerializer& operator << (OutputSerializer& ser, const URL& url) {
	ser << url.archiveName << url.relativePath;
	return ser;
}

_NexBaseAPI InputSerializer& operator >> (InputSerializer& ser, URL& url) {
	ser >> url.archiveName >> url.relativePath;
	return ser;
}

}

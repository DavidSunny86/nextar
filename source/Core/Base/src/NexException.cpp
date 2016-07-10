#include <BaseHeaders.h>

namespace nextar {

const char* Exception::GetMsg(const char* source) const {
	std::ostringstream location;
	static char buff[1024];
	const char* type[3] = { "Fatal", "Warning", "Graceful" };
	location << "Type: " << type[severity] << std::endl << "Error: "
			<< (KnownError) errorCode << std::endl << "File: " << file
			<< std::endl << "Function: " << function << std::endl << "Line: "
			<< line;
	if (source)
		location << " Source: " << source;
	location << std::endl;
	std::string formatted = location.str();
	std::memcpy(buff, formatted.c_str(), formatted.length());
	return buff;
}
}


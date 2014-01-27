#ifndef NEXTAR_NAMEVALPAIR_H
#define NEXTAR_NAMEVALPAIR_H

#include "NexSys.h"

namespace nextar {
	typedef std::pair<String, String> NameValuePair;
	typedef unordered_map<String, String>::type NameValueMap;
}

#endif //NEXTAR_NAMEVALPAIR_H

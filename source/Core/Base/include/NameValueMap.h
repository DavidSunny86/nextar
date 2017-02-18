#ifndef NEXTAR_NAMEVALPAIR_H
#define NEXTAR_NAMEVALPAIR_H

#include <NexBase.h>

namespace nextar {
	
typedef std::pair<String, String> NameValuePair;
// todo considerations to make it efficient
typedef map<String, String>::type NameValueMap;
// hash map instead of name value
typedef map<nextar::hash64, String>::type HashValueMap;

}

#endif //NEXTAR_NAMEVALPAIR_H
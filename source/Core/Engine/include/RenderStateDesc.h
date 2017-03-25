/*
 * RenderState.h
 *
 *  Created on: 04-Mar-2017
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_RENDERSTATEDESC_H_
#define CORE_ENGINE_INCLUDE_RENDERSTATEDESC_H_

#include <NexBase.h>
#include <RenderConstants.h>

namespace nextar {

template <typename Type>
class RenderStateDesc : public AllocGeneral {
public:
	inline RenderStateDesc() : hash(0) {}
	inline void UpdateHash() {
		hash = StringUtils::Hash((const char*)(this), sizeof(Type));
	}

	hash_t hash;
};

}



#endif /* CORE_ENGINE_INCLUDE_RENDERSTATEDESC_H_ */

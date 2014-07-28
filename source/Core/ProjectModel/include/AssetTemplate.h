/*
 * AssetTemplate.h
 *
 *  Created on: 19-Jul-2014
 *      Author: obhi
 */

#ifndef ASSETTEMPLATE_H_
#define ASSETTEMPLATE_H_

#include <Asset.h>

namespace nextar {

/*
 *
 */
class AssetTemplate: public Asset {
public:
	AssetTemplate(const StringID, const StringID);
	virtual ~AssetTemplate();

	inline void DoModification() {
		modification++;
	}

	inline void UndoModification() {
		modification--;
	}

	inline bool IsChanged() const {
		return modification != lastSavedModification;
	}

protected:
	std::chrono::time_point lastSaveTime;
	uint32 lastSavedModification;
	uint32 modification;
};

} /* namespace nextar */

#endif /* ASSETTEMPLATE_H_ */

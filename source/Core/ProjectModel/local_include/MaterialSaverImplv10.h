/*
 * MaterialSaverImplv10.h
 *
 *  Created on: 28-Jul-2014
 *      Author: obhi
 */

#ifndef MATERIALSAVERIMPLV10_H_
#define MATERIALSAVERIMPLV10_H_

#include <EngineHeaders.h>

namespace nextar {

/*
 *
 */
class MaterialSaverImplv10  : public AssetSaverImpl,
public AllocGeneral {
public:
	MaterialSaverImplv10();
	virtual ~MaterialSaverImplv10();

	virtual void Save(OutputStreamPtr&, AssetSaver&);

};

} /* namespace nextar */

#endif /* MATERIALSAVERIMPLV10_H_ */

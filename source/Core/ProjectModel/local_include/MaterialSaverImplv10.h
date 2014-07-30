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
class MaterialSaverImplv1_0  : public AssetSaverImpl,
public AllocGeneral {
public:
	static MaterialSaverImplv1_0 saver;
	MaterialSaverImplv1_0();
	virtual ~MaterialSaverImplv1_0();

	virtual void Save(OutputStreamPtr&, AssetSaver&);

};

} /* namespace nextar */

#endif /* MATERIALSAVERIMPLV10_H_ */

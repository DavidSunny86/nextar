/*
 * ShaderSaverImplv10.h
 *
 *  Created on: 21-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERSAVERIMPLV10_H_
#define SHADERSAVERIMPLV10_H_

#include <EngineHeaders.h>

namespace nextar {

/*
 *
 */
class ShaderSaverImplv10 : public AssetSaverImpl,
	public AllocGeneral {
	NEX_LOG_HELPER(ShaderSaverImplv10);
public:
	ShaderSaverImplv10();
	virtual ~ShaderSaverImplv10();

	virtual void Save(OutputStreamPtr&, AssetSaver&);

	void SavePass(const ShaderTemplate::PassUnit& pu, ChunkOutputStream& stream);
};

} /* namespace nextar */

#endif /* SHADERSAVERIMPLV10_H_ */

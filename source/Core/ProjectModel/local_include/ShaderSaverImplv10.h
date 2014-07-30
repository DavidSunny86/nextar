/*
 * ShaderSaverImplv10.h
 *
 *  Created on: 21-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERSAVERIMPLV10_H_
#define SHADERSAVERIMPLV10_H_

#include <EngineHeaders.h>
#include <ShaderTemplate.h>

namespace nextar {

/*
 *
 */
class ShaderSaverImplv1_0 : public AssetSaverImpl,
	public AllocGeneral {
	NEX_LOG_HELPER(ShaderSaverImplv1_0);
public:
	static ShaderSaverImplv1_0 saver;

	ShaderSaverImplv1_0();
	virtual ~ShaderSaverImplv1_0();

	virtual void Save(OutputStreamPtr&, AssetSaver&);

	void SavePass(const ShaderTemplate::PassUnit& pu, ChunkOutputStream& stream);
};

} /* namespace nextar */

#endif /* SHADERSAVERIMPLV10_H_ */

/*
 * MeshServices.h
 *
 *  Created on: 15-Jul-2014
 *      Author: obhi
 */

#ifndef MESHSERVICES_H_
#define MESHSERVICES_H_

#include <VertexElement.h>
#include <MeshAsset.h>
#include <StreamData.h>

namespace nextar {

/*
 *
 */
class MeshServices : public Singleton<MeshServices>,
	public AllocGraphics {
public:

	StreamData* GetUnitOmniVolume();
	StreamData* GetUnitSpotVolume();

	MeshServices();
	virtual ~MeshServices();

	void Configure(const Config& config);
	void Close();

	StreamData* CreateSphereMesh(uint32 density, float radius, bool normalData = false, bool uvData = false);

protected:

	void SafeDelete(StreamData*&);

	StreamData* unitOmniLV;
	StreamData* unitSpotLV;
};

} /* namespace nextar */

#endif /* MESHSERVICES_H_ */

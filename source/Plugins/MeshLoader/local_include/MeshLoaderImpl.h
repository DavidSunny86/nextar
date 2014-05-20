/* 
 * File:   MeshLoaderImplv1_0.h
 * Author: obhi
 *
 * Created on May 22, 2011, 9:55 AM
 */

#ifndef MESHLOADERIMPLV1_0_H
#define	MESHLOADERIMPLV1_0_H

#include <EngineHeaders.h>
#include <Mesh.h>

using namespace nextar;
namespace MeshLoader {

	class Plugin;
	class MeshLoaderIntf {
	public:
		virtual void Load(InputSerializer&, MeshAsset::Loader& loader) = 0;
	protected:
		~MeshLoaderIntf() {}
	};

	class MeshLoaderImpl: public AssetLoaderImpl, public AllocGeneral {
		NEX_LOG_HELPER(MeshLoaderImpl);
		static MeshLoaderImpl impl;
		friend class Plugin;
	public:
		MeshLoaderImpl();
		virtual ~MeshLoaderImpl();

		virtual void Load(InputStreamPtr&, AssetLoader& loader);
	};
}
#endif	/* MESHLOADERIMPLV1_0_H */


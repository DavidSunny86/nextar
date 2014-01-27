/* 
 * File:   MeshLoaderImplv1_0.h
 * Author: obhi
 *
 * Created on May 22, 2011, 9:55 AM
 */

#ifndef MESHLOADERIMPLV1_0_H
#define	MESHLOADERIMPLV1_0_H

#include "Mesh.h"

using namespace nextar;
namespace MeshLoader {

	struct MeshLoaderIntf {
		virtual void Load(InputSerializer&, Mesh::Loader& loader) = 0;
	};

	class MeshLoaderImpl: public Mesh::LoaderImpl, public AllocGeneral {
		NEX_LOG_HELPER(MeshLoaderImpl);

	public:
		MeshLoaderImpl();
		virtual ~MeshLoaderImpl();

		virtual void Load(InputStreamPtr&, Mesh::Loader& loader);
	};
}
#endif	/* MESHLOADERIMPLV1_0_H */


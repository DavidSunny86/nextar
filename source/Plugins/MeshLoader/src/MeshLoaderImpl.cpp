/* 
 * File:   MeshLoaderImplv1_0.cpp
 * Author: obhi
 * 
 * Created on May 22, 2011, 9:55 AM
 */
#include <BaseHeaders.h>
#include <MeshLoaderIntfv10.h>
#include <BufferManager.h>
#include <VertexElement.h>
#include <GenericStreams.h>

namespace MeshLoader {

	enum MeshFileMagic {
		FILE_MAGIC_MESH = 0xff811901
	};

	/*****************************************************/
	/* MeshLoaderImpl      								 */
	/*****************************************************/
	MeshLoaderImpl::MeshLoaderImpl() {
	}

	MeshLoaderImpl::~MeshLoaderImpl() {
	}

	void MeshLoaderImpl::Load(InputStreamPtr& input, MeshAsset::Loader& loader) {

		static MeshLoaderImplv1_0 loaderImplv1_0;
		StreamRequest* request = loader.GetRequestPtr();
		MeshAsset* mesh = static_cast<StreamRequest*>(request->GetStreamedObject());

		if (input) {

			InputSerializer ser(input);
			uint32 meshMagic;
			String versionInfo;
			MeshLoaderIntf* impl = 0;
			ser >> (meshMagic);
			if (meshMagic == FILE_MAGIC_MESH) {
				ser >> versionInfo;
				VersionID ver = Convert::ToVersion(versionInfo);
				switch (ver) {
				case NEX_MAKE_VERSION(1,0,0):
					impl = &loaderImplv1_0;
					break;
				default:
					Error( String("Unsupported mesh version in file: ")
									+ mesh->GetAssetLocator().ToString());
					NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
					break;
				}

				NEX_ASSERT(impl);
				impl->Load(ser, loader);
			}
		} else {
			Error(
					String("Could not open mesh file: ")
							+ mesh->GetAssetLocator().ToString());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOCATE_ASSET);
		}
	}
}

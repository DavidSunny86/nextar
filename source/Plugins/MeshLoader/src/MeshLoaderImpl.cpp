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

MeshLoaderImpl MeshLoaderImpl::impl;
/*****************************************************/
/* MeshLoaderImpl      								 */
/*****************************************************/
MeshLoaderImpl::MeshLoaderImpl() {
}

MeshLoaderImpl::~MeshLoaderImpl() {
}

void MeshLoaderImpl::Load(InputStreamPtr& input, AssetLoader& loader) {

	static MeshLoaderIntfv1_0 loaderImplv1_0;
	AssetStreamRequest* request = loader.GetRequestPtr();
	MeshAsset* mesh = static_cast<MeshAsset*>(request->GetStreamedObject());

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
			case NEX_MAKE_VERSION(1, 0, 0):
				impl = &loaderImplv1_0;
				break;
			default:
				Error(
						String("Unsupported mesh version in file: ")
								+ request->GetName());
				request->SetCompleted(false);
				break;
			}

			NEX_ASSERT(impl);
			try {
				impl->Load(ser, loader);
				request->SetCompleted(true);
			}
			catch (GracefulErrorExcept& e) {
				request->SetCompleted(false);
			}
		}
	} else {
		Error(
				String("Could not open mesh file: ")
						+ request->GetName());
		request->SetCompleted(false);
	}
}
}

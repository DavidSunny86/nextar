/*
 * ResourcePackerServiceImpl.cpp
 *
 *  Created on: Sep 18, 2016
 *      Author: obhi
 */

#include <ResourcePackerServiceImpl.h>

namespace ResourcePacker {

ResourcePackerServiceImpl::ResourcePackerServiceImpl() : refNumber(0) {
}

ResourcePackerServiceImpl::~ResourcePackerServiceImpl() {
}

void ResourcePackerServiceImpl::BuildMaterials(
		const ProjectContextPtr& context) {
	ArchivePtr arch = FileSystem::Instance().OpenArchive(FileSystem::ArchiveProjectData_Name);
	if (arch) {
		MaterialLoad loader;
		String mpath = context->GetResourcePath(ProjectResourceType::RESOURCE_MATERIAL);
		mpath += "/*.mscript";
		arch->Scan(&loader,  mpath);
	}
}

void ResourcePackerServiceImpl::BuildRenderScripts(
		const ProjectContextPtr& context) {
	ArchivePtr arch = FileSystem::Instance().OpenArchive(FileSystem::ArchiveProjectData_Name);
	if (arch) {
		MaterialLoad loader;
		String mpath = context->GetResourcePath(ProjectResourceType::RESOURCE_RENDERSCRIPT);
		mpath += "/*.rscript";
		arch->Scan(&loader,  mpath);
	}
}

void ResourcePackerServiceImpl::BuildProject(const ProjectContextPtr& context) {
	BuildMaterials(context);
	BuildRenderScripts(context);
}

void ResourcePackerServiceImpl::Destroy() {
	DecrementUsage();
}

void ResourcePacker::ResourcePackerServiceImpl::MaterialLoad::FoundFile(
		const FileAttribute& attribute, Archive*) {
	Trace("File: " + attribute.fileName.ToString());
	String materialName = attribute.fileName.GetComputedName();

	MaterialTemplate::ID id(NamedObject::AsyncStringID(materialName));
	URL materialSaveUrl("{ProjectData}/Materials/" + materialName + ".nexmtl");
	URL materialAssetSaveUrl("{ProjectData}/Materials/Assets/" + materialName + ".asset");
	MaterialTemplatePtr material = MaterialTemplate::Traits::Instance(id, attribute.fileName);
	material->RequestLoad();
	if (material->AsyncIsLoaded()) {
		ShaderTemplatePtr shader = material->GetShaderTemplate();
		if (shader) {
			URL locator = shader->GetAssetLocator();
			URL shaderSaveUrl("{ProjectData}/Shaders/" + locator.GetComputedName() + ".nexfx");
			shader->SetAssetLocator(shaderSaveUrl);
			material->SetAssetLocator(materialSaveUrl);
			shader->RequestSave();
			material->RequestSave();
			AssetPtr asset = material;
			Asset::AssetSave(asset, materialAssetSaveUrl, "nexmtl");
			return;
		}
	}
	Error("Failed to load: " + attribute.fileName.ToString());
}

void ResourcePacker::ResourcePackerServiceImpl::RenderScriptLoad::FoundFile(
		const FileAttribute& attribute, Archive*) {
	String renderScript = attribute.fileName.ToString();
	Trace("File: " + renderScript);


	RenderSystemPtr rsys = RenderManager::Instance().CreateRenderSystem(renderScript,
			Size(800, 600));
	URL saveUrl("{ProjectData}/Config/" + attribute.fileName.GetComputedName() + ".nexrsys");
	OutputStreamPtr output = FileSystem::Instance().OpenWrite(saveUrl);
	rsys->Save(output, "nexrsys");
}

} /* namespace ResourcePacker */

#include "BaseHeaders.h"
#include "PluginRegistry.h"
#include "URL.h"
#include "Config.h"
#include "NexXml.h"
#include "FileSystem.h"

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(PluginRegistry);

	PluginRegistry::DynLib::DynLib(const String& path, const String& plug,
			const PluginLicense& license,uint32 build, bool plgOptional) :
			optional(plgOptional), name(plug), libptr(0), plugin(0), buildVersion(build) {
		fullName = URL::GetAppendedPath(path, NEX_MODULE_NAME(name));
		this->license = license;
	}

	PluginRegistry::DynLib::~DynLib() {
		if (libptr)
			_UnloadLib();
	}

	void PluginRegistry::DynLib::Request(PluginLicenseType pluginType,
			const String& name,
			bool load) {

		if (license.type == pluginType) {
			if (name.length() && name != license.customType)
				return;
			if (load) {

				if (!plugin && !_LoadLib()) {
					Warn(String("Plugin failed to load: ") + name);
					if (!optional)
						NEX_THROW_FatalError(EXCEPT_FAILED_TO_LOADLIB);
				}

				plugin->LicenseRenewed();

			} else {

				if (plugin && plugin->LicenseExpired())
					_UnloadLib();
			}
		}
	}

	bool PluginRegistry::DynLib::_LoadLib() {
		if (plugin)
			return true;
		if (!libptr)
			libptr = NEX_DYN_LOAD(fullName.c_str());
		if (!libptr) {
			Error(
					String("DYN_LOAD failed with this error: ")
							+ String(NEX_DYN_ERR()));
			return false;
		}

		String factoryname = name + "CreateFactory";
		PluginCreateFactory fac =
				reinterpret_cast<PluginCreateFactory>(NEX_DYN_LOAD_SYMBOL(libptr,
						factoryname.c_str()));

		if (!fac) {
			factoryname.append("_", 0, 1);
			fac =
					reinterpret_cast<PluginCreateFactory>(NEX_DYN_LOAD_SYMBOL(libptr, factoryname.c_str()));
		}

		if (fac && (plugin = fac())) {
			plugin->Open();
			Trace(String("Opened lib: ") + name);
			return true;
		}
		return false;
	}

	void PluginRegistry::DynLib::_UnloadLib() {
		if (!libptr)
			return;
		if (plugin) {
			Trace(String("Closed lib: ") + name);
			plugin->Close();
			plugin->Dispose();
		}

		plugin = 0;
		NEX_DYN_FREE(libptr);
		libptr = 0;
	}

	PluginRegistry::PluginRegistry() {
	}

	PluginRegistry::~PluginRegistry() {
		nextar::DisposeRange<DisposerDelete<DynLib> >(libraries.begin(),
				libraries.end());
	}

	PluginLicenseType PluginRegistry::_ParsePluginLicenseType(const String& type) {
		if (type == "Lifetime")
			return PLUG_TYPE_LIFETIME;
		else if (type == "Graphics")
			return PLUG_TYPE_GRAPHICS;
		else
			return PLUG_TYPE_CUSTOM;
	}

	void PluginRegistry::Configure(const Config& config) {

		String configFile = config.GetValue("Engine", "PluginConfigurations",
				"${Bin}/Plugins.xml");
		_ParsePluginConfiguration(configFile);
	}

	void PluginRegistry::_ParsePluginConfiguration(const URL& path) {
		FileSystem& fileSys = FileSystem::Instance();
		InputStreamPtr xmlFile = fileSys.OpenRead(path);
		if (xmlFile) {
			xml::Document document;
			try {
				document.Load(xmlFile);
				xml::Element* root = document.GetFirstElement();
				URL basePath = root->GetAttributeValue("basePath", "${Bin}");
				pluginSearchPath = basePath.GetRelativePath();

				xml::Node::Iterator it = root->FirstChildIt();
				while(it.IsOk()) {
					if (it->GetValue() == "plugin") {
						// parse this plugin
						PluginLicense license;
						license.customType = it->GetText("type", "Custom");
						license.type = _ParsePluginLicenseType(license.customType);
						AddPlugin(it->GetText("path"),
								it->GetText("name"),
								license,
								Convert::ToVersion(it->GetText("build", NEX_VERSION_STRING())),
								Convert::ToBool(it->GetText("optional", "true")));
						it = it.Next();
					}
				}
			} catch (Exception& e) {
				// parse failed, so just return;
				Error(String("Was unable to load plugin configurations due to: ") + e.GetMsg());
				return;
			}
		}
	}

	void PluginRegistry::AddPlugin(const String& path, const String& name,
			const PluginLicense& license, uint32 version, bool optional) {

		if (name.length() <= 0)
			return;

		for (size_t i = 0; i < libraries.size(); ++i)
			if (libraries[i]->GetName() == name)
				return;
		libraries.push_back(
				NEX_NEW DynLib(URL::GetAppendedPath(pluginSearchPath, path),
						name, license, version, optional));
	}

	void PluginRegistry::RequestPlugins(PluginLicenseType le,
			const String& typeName,
			bool loadPlugins) {

		for (size_t i = 0; i < libraries.size(); ++i)
			libraries[i]->Request(le, typeName, loadPlugins);
	}
}


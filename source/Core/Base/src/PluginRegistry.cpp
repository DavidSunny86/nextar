#include <BaseHeaders.h>
#include <PluginRegistry.h>
#include <URL.h>
#include <Config.h>
#include <NexXml.h>
#include <FileSystem.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(PluginRegistry);

PluginRegistry ::DynLib::DynLib(const String& path, const String& plug,
	const String& serviceName,
	const PluginLicense& license, uint32 build, ApplicationContextType contextType, bool plgOptional) :
		fullName(path),
		optional(plgOptional), name(plug), serviceNames(serviceName), libptr(0), plugin(0), context(contextType), buildVersion(build) {
	this->license = license;
}

PluginRegistry::DynLib::~DynLib() {
	if (libptr)
		_UnloadLib();
}

void PluginRegistry::DynLib::Request(
	bool load) {
	if (load) {
		if (!plugin) {
			if (!_LoadLib()) {
				Warn(String("Plugin failed to load: ") + this->name);
				if (!optional)
					NEX_THROW_FatalError(EXCEPT_FAILED_TO_LOADLIB);
			}
			plugin->LicenseRenewed();
		}
	} else {

		if (plugin && plugin->LicenseExpired())
			_UnloadLib();
	}
}

void PluginRegistry::DynLib::Request(PluginLicenseType pluginType,
		const String& name,
		bool load) {

	if (license.type == pluginType) {
		if (name.length() && name != license.customType)
			return;
		Request(load);
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
		fac = reinterpret_cast<PluginCreateFactory>(NEX_DYN_LOAD_SYMBOL(libptr,
				factoryname.c_str()));
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

String PluginRegistry::DynLib::GetImpl(const char* serv) {
	size_t p = serviceNames.find(name);
	if (p != String::npos) {
		p = serviceNames.find_first_of(':', p);
		if (p != String::npos) {
			size_t en = serviceNames.find_first_of(';', p);
			if (en != String::npos)
				en = en - (p+1);
			return serviceNames.substr(p+1, en);
		}
	}
	return StringUtils::Null;
}

PluginService* PluginRegistry::DynLib::Query(const String& name, const String& impl) {
	size_t p = serviceNames.find(name);
	if (p != String::npos) {
		p = serviceNames.find_first_of(':', p);
		if (p != String::npos) {
			if (!impl.length() || !serviceNames.compare(p+1, impl.size(), impl)) {
				Request(true);
				if (plugin)
					return plugin->Query(name, impl);
			}
		}
	}

	return nullptr;
}

PluginRegistry::PluginRegistry() {
}

PluginRegistry::~PluginRegistry() {
	nextar::DisposeRange<DisposerDelete<DynLib> >(libraries.begin(),
			libraries.end());
}

PluginLicenseType PluginRegistry::_ParsePluginLicenseType(const String& type) {
	if (type == "Preload")
		return PLUG_TYPE_PRELOAD;
	else
		return PLUG_TYPE_CUSTOM;
}

void PluginRegistry::Configure(const Config& config) {

	String configFile = config.GetValue("Engine", "PluginConfigurations",
			"${Bin}/Plugins.xml");
	_ParseAndLoadPlugins(configFile);
}

void PluginRegistry::AddPlugins(const URL& file) {
	_ParseAndLoadPlugins(file);
}

void PluginRegistry::_ParseAndLoadPlugins(const URL& file) {
	_ParsePluginConfiguration(file);
	RequestPlugins(PLUG_TYPE_PRELOAD,
				StringUtils::Null, true);
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
			while (it.IsOk()) {
				if (it->GetValue() == "plugin") {
					// parse this plugin
					PluginLicense license;
					license.customType = it->GetText("type", "Custom");
					license.type = _ParsePluginLicenseType(license.customType);
					
					AddPlugin(NEX_MODULE_NAME(it->GetText("target")), 
						static_cast<xml::Element*>(*it)->GetAttributeValue("name"), 
						it->GetText("services", StringUtils::Null),
						license,
						Convert::ToVersion(
								it->GetText("build", NEX_VERSION_STRING())),
						Convert::ToApplicationContext(
								it->GetText("context", "unknown")),
						Convert::ToBool(it->GetText("optional", "true")));
					it = it.Next();
				}
			}
		} catch (Exception& e) {
			// parse failed, so just return;
			Error(
					String("Was unable to load plugin configurations due to: ")
							+ e.GetMsg());
			return;
		}
	}
}

void PluginRegistry::AddPlugin(const String& path, const String& name,
	const String& services,
	const PluginLicense& license, uint32 version, 
	ApplicationContextType type, bool optional) {

	if (name.length() <= 0)
		return;

	for (size_t i = 0; i < libraries.size(); ++i)
		if (libraries[i]->GetName() == name)
			return;
	libraries.push_back(
			NEX_NEW(
			DynLib(URL::GetAppendedPath(pluginSearchPath, path), name, services, license, version, type, optional)));
}

void PluginRegistry::RequestPlugins(PluginLicenseType le,
		const String& typeName,
		bool loadPlugins) {
	ApplicationContextType type = ApplicationContext::Instance().GetType();
	for (size_t i = 0; i < libraries.size(); ++i)
		if (libraries[i]->IsAccepted(type))
			libraries[i]->Request(le, typeName, loadPlugins);
}

PluginService* PluginRegistry::QueryService(const char* name, const char* impl) {
	String serviceName = name;
	String serviceImpl = (!impl || impl[0] == 0) ? StringUtils::Null : impl;
	for (size_t i = 0; i < libraries.size(); ++i) {
		PluginService* service = libraries[i]->Query(serviceName, serviceImpl);
		if (service)
			return service;
	}
	return nullptr;
}

StringUtils::WordList PluginRegistry::EnumImplementations(const char* serviceName) {
	StringUtils::WordList serviceImpl;
	for (size_t i = 0; i < libraries.size(); ++i) {
		String impl = libraries[i]->GetImpl(serviceName);
		if (impl.length() > 0)
			StringUtils::PushBackWord(serviceImpl, impl);
	}
	return serviceImpl;
}

void PluginRegistry::UnloadPlugins() {
	RequestPlugins(PLUG_TYPE_PRELOAD,
			StringUtils::Null, false);
	RequestPlugins(PLUG_TYPE_CUSTOM,
				StringUtils::Null, false);
}

}


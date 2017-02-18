/*
 * RenderSystemImpl.cpp
 *
 *  Created on: 06-Feb-2016
 *      Author: obhi
 */

#include <RenderSystemImpl.h>
#include <RenderManager.h>
#include <ApplicationContext.h>
#include <BaseRenderPass.h>

namespace nextar {

RenderSystemImpl::RenderSystemImpl(Size size) :
RenderSystem(),
currentSize(size),
metaInfo(nullptr),
flags(0) {
}

RenderSystemImpl::~RenderSystemImpl() {
	if (metaInfo)
		NEX_DELETE(metaInfo);
	for (auto& p : renderPasses) {
		NEX_DELETE(p);
	}
	ApplicationContext::Instance().Unsubscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources);
}

void RenderSystemImpl::RegisterTarget(StringID as,
	RenderTargetPtr target) {
	targets[as].target = target;
}

RenderTargetPtr RenderSystemImpl::GetTarget(StringID name) {
	auto it = targets.find(name);
	if (it != targets.end())
		return (*it).second.target;
	return RenderTargetPtr();
}

StringID RenderSystemImpl::GetTargetName(RenderTarget* name) {
	for (auto &e : targets) {
		if (e.second.target.GetPtr() == name) {
			return e.first;
		}
	}
	return StringUtils::NullID;
}

void RenderSystemImpl::Load(InputStreamPtr& stream, const String& fileType) {
	RenderSystem::Streamer* streamer = RenderManager::Instance().GetRenderStreamer(fileType);
	if (streamer)
		streamer->Load(*this, stream);
	else
		Error("Failed to load render system for type: " + fileType);
}

void RenderSystemImpl::Save(OutputStreamPtr& stream, const String& fileType) {
	RenderSystem::Streamer* streamer = RenderManager::Instance().GetRenderStreamer(fileType);
	if (streamer)
		streamer->Save(*this, stream);
	else
		Error("Failed to save render system for type: " + fileType);
}

void RenderSystemImpl::BeginConfig(bool bStoreMetaInfo) {
	renderSystemTicket = (uint32)(ptrdiff_t)this;
	if (bStoreMetaInfo)
		metaInfo = NEX_NEW(MetaInfo);
}

StringID RenderSystemImpl::GetName(const RenderTargetPtr& rt) {
	for (auto& t : targets) {
		if (t.second.target.GetPtr() == rt.GetPtr())
			return t.first;
	}
	return StringUtils::NullID;
}

RenderTargetPtr RenderSystemImpl::CreateMultiRenderTarget(StringID name,
	const MultiRenderTarget::CreateParams& iparams,
	float dx, float dy) {

	renderSystemTicket++;

	MultiRenderTarget::CreateParams params = iparams;
	MultiRenderTargetPtr renderTarget = Assign(NEX_NEW(MultiRenderTarget()));
	if (params.dimensions.combined == 0) {
		// factored
		params.dimensions.dx = (uint16)(currentSize.dx * dx);
		params.dimensions.dy = (uint16)(currentSize.dy * dy);
		flags |= HAS_RELATIVE_TARGETS;
	}

	renderTarget->Create(params);
	auto& e = targets[name];
	e.target = renderTarget;
	e.viewRelativeX = dx;
	e.viewRelativeY = dy;

	if (metaInfo) {
		if (!metaInfo->ValidateDimensions(iparams.dimensions.dx, iparams.dimensions.dy, 1)) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_ARGS);
		}

		BufferInfo info;
		info.dx = dx;
		info.dy = dy;
		info.format = PixelFormat::UNKNOWN;
		info.name = name;
		info.samples = 0;
		info.type = RenderTargetType::MULTI_RENDER_TARGET;
		info.mrtData = NEX_NEW(MultiRenderTargetData);
		if (iparams.useDepth) {
			info.mrtData->depth.ref = GetName(iparams.depth.useTarget);
			info.mrtData->depth.format = iparams.depth.format;
			info.mrtData->depth.useAsTarget = iparams.depth.useAsTexture;
		}

		info.mrtData->numColorBuffer = iparams.numColorTargets;
		for (uint32 i = 0; i < iparams.numColorTargets; ++i) {
			info.mrtData->color[i].ref = GetName(iparams.targets[i].useTarget);
			info.mrtData->color[i].format = iparams.targets[i].format;
			info.mrtData->color[i].useAsTarget = iparams.targets[i].useAsTexture;
		}

		metaInfo->AddBuffer(std::move(info));
	}

	return e.target;
}

RenderTargetPtr RenderSystemImpl::CreateRenderTexture(StringID name,
	const RenderTexture::CreateParams& iparams,
	float dx, float dy) {

	renderSystemTicket++;

	RenderTexture::CreateParams params = iparams;
	RenderTexturePtr renderTarget = Assign(NEX_NEW(RenderTexture()));
	if (params.width == 0 && params.height == 0) {
		// factored
		// assert depth is 1??
		params.width = (uint16)(currentSize.dx * dx);
		params.height = (uint16)(currentSize.dy * dy);
		flags |= HAS_RELATIVE_TARGETS;
	}

	renderTarget->Create(params);
	auto& e = targets[name];
	e.target = renderTarget;
	e.viewRelativeX = dx;
	e.viewRelativeY = dy;

	if (metaInfo) {
		if (!metaInfo->ValidateDimensions(iparams.width, iparams.height, iparams.depth)) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_ARGS);
		}
		BufferInfo info;
		info.dx = dx;
		info.dy = dy;
		info.format = iparams.format;
		info.name = name;
		info.samples = 0;
		info.type = RenderTargetType::TEXTURE;
		metaInfo->AddBuffer(std::move(info));
	}

	return e.target;
}

RenderTargetPtr RenderSystemImpl::CreateRenderBuffer(StringID name,
	const RenderBuffer::CreateParams& iparams,
	float dx,
	float dy) {

	renderSystemTicket++;

	RenderBuffer::CreateParams params = iparams;
	RenderBufferPtr renderTarget = Assign(NEX_NEW(RenderBuffer()));
	if (params.dimensions.combined == 0) {
		// factored
		params.dimensions.dx = (uint16)(currentSize.dx * dx);
		params.dimensions.dy = (uint16)(currentSize.dy * dy);
		flags |= HAS_RELATIVE_TARGETS;
	}

	renderTarget->Create(params);
	auto& e = targets[name];
	e.target = renderTarget;
	e.viewRelativeX = dx;
	e.viewRelativeY = dy;

	if (metaInfo) {
		if (!metaInfo->ValidateDimensions(iparams.dimensions.width, iparams.dimensions.height, 1)) {
			NEX_THROW_GracefulError(EXCEPT_INVALID_ARGS);
		}
		BufferInfo info;
		info.dx = dx;
		info.dy = dy;
		info.format = iparams.format;
		info.name = name;
		info.samples = iparams.samples;
		info.type = RenderTargetType::RENDER_BUFFER;
		metaInfo->AddBuffer(std::move(info));
	}
	return e.target;
}

RenderPass* RenderSystemImpl::AddPass(const String& type) {

	renderSystemTicket++;
	RenderPass* r = RenderManager::Instance().CreateRenderPass(
		StringUtils::GetStringID(type));
	renderPasses.push_back(r);
	return r;
}

void RenderSystemImpl::EndConfig() {

	renderSystemTicket++;
}

uint32 RenderSystemImpl::GetRenderTargetCount() const {
	return (uint32)targets.size();
}

void RenderSystemImpl::Commit(CommitContext& context) {
	if (context.viewDimensions.combined != currentSize.combined) {
		if (flags & HAS_RELATIVE_TARGETS) {
			for (auto& e : targets) {
				if (e.second.viewRelativeX > 0 &&
					e.second.viewRelativeY > 0) {
					Size newDim = Size((uint16)(context.viewDimensions.dx * e.second.viewRelativeX),
						(uint16)(context.viewDimensions.dy * e.second.viewRelativeY));
					e.second.target->Resize(newDim);
				}
			}
		}
		currentSize = context.viewDimensions;
	}

	context.rsys = this;
	for (auto& p : renderPasses) {
		if (p->IsEnabled())
			p->Commit(context);
	}
}

RenderPass* RenderSystemImpl::FindPass(StringID name) {
	for (auto& p : renderPasses) {
		if (p->GetID() == name)
			return p;
	}
	return nullptr;
}

void RenderSystemImpl::EnablePass(StringID pass) {

	renderSystemTicket++;

	RenderPass* p = FindPass(pass);
	if (p)
		static_cast<BaseRenderPass*>(p)->SetEnabled(true);
}

void RenderSystemImpl::DisablePass(StringID pass) {

	renderSystemTicket++;

	RenderPass* p = FindPass(pass);
	if (p)
		static_cast<BaseRenderPass*>(p)->SetEnabled(false);
}

void RenderSystemImpl::CreateResources() {
	renderSystemTicket++;

	for (auto& p : renderPasses) {
		static_cast<BaseRenderPass*>(p)->CreateResources();
	}
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
}

void RenderSystemImpl::DestroyResources() {

	renderSystemTicket++;

	for (auto& p : renderPasses) {
		if (static_cast<BaseRenderPass*>(p)->IsEnabled())
			static_cast<BaseRenderPass*>(p)->DestroyResources();
	}
}

void RenderSystemImpl::DestroyResources(void* thisPointer) {
	reinterpret_cast<RenderSystemImpl*>(thisPointer)->DestroyResources();
}

uint32 RenderSystemImpl::GetPassCount() const {
	return (uint32)renderPasses.size();
}

RenderPass* RenderSystemImpl::GetPass(uint32 i) {
	return renderPasses[i];
}

void RenderSystemImpl::DefaultStreamer::_WriteVersion(OutputStreamPtr& stream) {
	OutputSerializer ser(stream);
	uint32 magic = FILE_MAGIC_RSCRIPT;
	VersionID ver = NEX_MAKE_VERSION(1, 0, 0);
	String versionInfo = Convert::ToVersionString(ver);
	ser << magic << versionInfo;
}

bool RenderSystemImpl::DefaultStreamer::_ReadVersion(InputSerializer& ser) {
	uint32 magic;
	String versionInfo;
	ser >> magic >> versionInfo;
	VersionID ver = Convert::ToVersion(versionInfo);
	const VersionID accpeted = NEX_MAKE_VERSION(1, 0, 0);
	return (magic == FILE_MAGIC_RSCRIPT) && TestVersion(accpeted, ver);
}

void RenderSystemImpl::DefaultStreamer::_WritePass(RenderSystem* s, 
	RenderPass* pass, ChunkOutputStream& ostr) {
	OutputSerializer& ser = ostr.BeginChunk(RSCRIPT_BEGIN_PASS);
	BaseRenderPass* basePass = static_cast<BaseRenderPass*>(pass);
	String name = basePass->GetPassType();
	StringID nameId = basePass->GetID();
	ser << name;
	ser << nameId;
	basePass->Save(s, ser);
	ostr.EndChunk();
}

void RenderSystemImpl::DefaultStreamer::_ReadPass(RenderSystem* s,
	ChunkInputStream& vostr) {

	InputSerializer::Chunk chunk = ChunkInputStream::First;
	ChunkInputStream& ostr = vostr.ReadChunk(RSCRIPT_BEGIN_PASS, chunk, chunk);
	if (InputSerializer::IsValid(chunk)) {
		String className;
		StringID name;

		ostr >> className >> name;
		BaseRenderPass* basePass = static_cast<BaseRenderPass*>(s->AddPass(className));
		if (!basePass) {
			Error(className + " is not a valid render pass!");
			NEX_THROW_GracefulError(EXCEPT_FAILED_TO_CREATE_OBJECT);
		}
			
		basePass->SetID(name);
		basePass->Load(s, ostr);
	}
}

void RenderSystemImpl::DefaultStreamer::_WriteBuffers(RenderSystem* s,
	const RenderSystemImpl::MetaInfo& m, OutputSerializer& ostr) {
		
	uint32 bufferCount = (uint32)m.bufferInfo.size();
	ostr << m.width << m.height << m.depth << bufferCount;

	for (auto& e : m.bufferInfo) {
		uint32 pixelFormat = (uint32)e.format;
		uint32 type = (uint32)e.type;
		ostr << e.name << e.dx << e.dy << pixelFormat << e.samples << type;
		if (e.mrtData) {
			uint16 numColorBuffer = e.mrtData->numColorBuffer;
#define NEX_DUMP_BUFFER_INFO(info) info.ref << static_cast<uint32>(info.format) << info.useAsTarget
			ostr << true << numColorBuffer << NEX_DUMP_BUFFER_INFO(e.mrtData->depth);
			for (uint32 i = 0; i < numColorBuffer; ++i) {
				ostr << NEX_DUMP_BUFFER_INFO(e.mrtData->color[i]);
			}
#undef NEX_DUMP_BUFFER_INFO
		} else
			ostr << false;
	}
}

void RenderSystemImpl::DefaultStreamer::_ReadBuffers(RenderSystem* s,
	InputSerializer& ostr) {

	bool metaInfo;
	ostr >> metaInfo;
	if (!metaInfo)
		return;
	uint32 width, height, depth, bufferCount;
	ostr >> width >> height >> depth >> bufferCount;
	for (uint32 i = 0; i < bufferCount; ++i) {
		StringID name;
		float dx, dy;
		uint32 pixelFormat, type;
		uint16 samples;
		bool hasMrtData;
		ostr >> name >> dx >> dy >> pixelFormat >> samples >> type >> hasMrtData;
		switch ((RenderTargetType)type) {
		case RenderTargetType::TEXTURE: {
			RenderTexture::CreateParams params;
			params.depth = depth;
			params.width = width;
			params.height = height;
			params.type = TextureBase::TextureType::TEXTURE_2D;
			params.format = (PixelFormat)pixelFormat;
			s->CreateRenderTexture(name, params, dx, dy);
		}
			break;
		case RenderTargetType::RENDER_BUFFER: {
			RenderBuffer::CreateParams params;
			params.format = (PixelFormat)pixelFormat;
			params.dimensions = Size(width, height);
			// @todo Set samples
			params.samples = samples;
			s->CreateRenderBuffer(name, params, dx, dy);
		}
			break;
		case RenderTargetType::MULTI_RENDER_TARGET: {
			
			uint16 numColorBuffer;
			MultiRenderTarget::CreateParams params;
			params.dimensions = Size(width, height);
			
			if (hasMrtData) {
				ostr >> numColorBuffer;
				StringID subName;
				//PixelFormat subFormat;
				bool useAsTexture = false;
#define NEX_READ_BUFFER_INFO(targParams) { \
					ostr >> subName >> pixelFormat >> useAsTexture; targParams.format = (PixelFormat)pixelFormat; \
					targParams.useAsTexture = useAsTexture; targParams.useTarget = s->GetTarget(subName); \
				}

				NEX_READ_BUFFER_INFO(params.depth);
				if (params.depth.useTarget)
					params.useDepth = true;
				params.numColorTargets = numColorBuffer;
				for (uint32 i = 0; i < numColorBuffer; ++i) {
					NEX_READ_BUFFER_INFO(params.targets[i]);
				}
#undef NEX_READ_BUFFER_INFO

				s->CreateMultiRenderTarget(name, params, dx, dy);
			} else {
				Warn("MRT could not be created, invalid data!");
			}
			
		}
			break;
		}
	}

}

bool RenderSystemImpl::DefaultStreamer::Save(RenderSystem& s, OutputStreamPtr& stream) {

	_WriteVersion(stream);
	ChunkOutputStream ostr(stream);

	OutputSerializer& bufSer = ostr.BeginChunk(RSCRIPT_BUFFER);
	RenderSystemImpl* impl = static_cast<RenderSystemImpl*>(&s);
	if (impl->metaInfo) {
		bufSer << true;
		_WriteBuffers(&s, *impl->metaInfo, bufSer);
	} else
		bufSer << false;
	ostr.EndChunk();

	OutputSerializer& ser = ostr.BeginChunk(RSCRIPT_PASS_DATA);

	uint32 passCount = s.GetPassCount();
	ser << passCount;
	for (uint32 i = 0; i < passCount; ++i) {
		_WritePass(&s, s.GetPass(i), ostr);
	}

	ostr.EndChunk();


	return true;
}

bool RenderSystemImpl::DefaultStreamer::Load(RenderSystem& s, InputStreamPtr& stream) {
	ChunkInputStream ser(stream);
	if (!_ReadVersion(ser))
		return false;
	InputSerializer::Chunk chunk = ChunkInputStream::First;

	ChunkInputStream& buffStr = ser.ReadChunk(RSCRIPT_BUFFER, chunk, chunk);
	if (InputSerializer::IsValid(chunk)) {
		_ReadBuffers(&s, buffStr);
	} else
		return false;

	ChunkInputStream& passStr = ser.ReadChunk(RSCRIPT_PASS_DATA, chunk, chunk);
	if (InputSerializer::IsValid(chunk)) {
		uint32 passCount;
		passStr >> passCount;
		for (uint32 i = 0; i < passCount; ++i) {
			_ReadPass(&s, passStr);
		}
	} else
		return false;

	return true;
}

/***************************************************************/
/*                     BufferInfo                              */
/***************************************************************/
RenderSystemImpl::BufferInfo::BufferInfo() :
name(StringUtils::NullID),
type(RenderTargetType::UNKNOWN_TYPE),
samples(0),
dx(0.0f), dy(0.0f),
format(PixelFormat::UNKNOWN),
mrtData(nullptr) {

}

RenderSystemImpl::BufferInfo::BufferInfo(const BufferInfo& info) :
name(info.name),
type(info.type),
samples(info.samples),
dx(info.dx), dy(info.dy),
format(info.format),
mrtData(nullptr) {
	if (info.mrtData) {
		mrtData = NEX_NEW(MultiRenderTargetData);
		*mrtData = *info.mrtData;
	}
}

RenderSystemImpl::BufferInfo::BufferInfo(BufferInfo&& info) :
name(info.name),
type(info.type),
samples(info.samples),
dx(info.dx), dy(info.dy),
format(info.format),
mrtData(info.mrtData) {
	info.mrtData = nullptr;
}

RenderSystemImpl::BufferInfo& RenderSystemImpl::BufferInfo::operator = (const RenderSystemImpl::BufferInfo& info) {
	name = info.name;
	type = info.type;
	samples = info.samples;
	dx = info.dx;
	dy = info.dy;
	format = info.format;
	if (!mrtData)
		mrtData = NEX_NEW(MultiRenderTargetData);
	*mrtData = *info.mrtData;
	return *this;
}

RenderSystemImpl::BufferInfo& RenderSystemImpl::BufferInfo::operator = (RenderSystemImpl::BufferInfo&& info) {
	name = info.name;
	type = info.type;
	samples = info.samples;
	dx = info.dx;
	dy = info.dy;
	format = info.format;
	if (mrtData)
		NEX_DELETE(mrtData);
	mrtData = info.mrtData;
	info.mrtData = nullptr;
	return *this;
}

RenderSystemImpl::BufferInfo::~BufferInfo() {
	if (mrtData)
		NEX_DELETE(mrtData);
}

/***************************************************************/
/*                     MetaInfo                                */
/***************************************************************/
RenderSystemImpl::MetaInfo::MetaInfo() : width(0), height(0), depth(0) {
}

bool RenderSystemImpl::MetaInfo::ValidateDimensions(uint32 iwidth, uint32 iheight, uint32 idepth) {
	if (width) {
		if (width != iwidth) {
			return false;
		}
	} else
		width = iwidth;

	if (height) {
		if (height != iheight) {
			return false;
		}
	} else
		height = iheight;

	if (depth) {
		if (depth != idepth) {
			return false;
		}
	} else
		depth = idepth;
	return true;
}

void RenderSystemImpl::MetaInfo::AddBuffer(RenderSystemImpl::BufferInfo&& info) {
	bufferInfo.push_back(std::move(info));
}

} /* namespace nextar */

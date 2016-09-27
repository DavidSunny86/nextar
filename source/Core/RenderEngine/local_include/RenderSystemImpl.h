/*
 * RenderSystemImpl.h
 *
 *  Created on: 06-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_
#define CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_

#include <NexRenderEngine.h>
#include <RenderSystem.h>
#include <MultiRenderTarget.h>
#include <RenderBuffer.h>

namespace nextar {

class RenderSystemImpl: public RenderSystem {
	NEX_LOG_HELPER(RenderSystemImpl);
public:
		
	
	RenderSystemImpl(Size size);
	virtual ~RenderSystemImpl();

	virtual void RegisterTarget(StringID as, RenderTargetPtr target) override;
	virtual RenderTargetPtr GetTarget(StringID name) override;
	virtual StringID GetTargetName(RenderTarget* name) override;

	virtual void Load(InputStreamPtr& stream, const String& fileType);
	virtual void Save(OutputStreamPtr& stream, const String& fileType);

	virtual void BeginConfig(bool bStoreMetaInfo = false);
	virtual RenderTargetPtr CreateMultiRenderTarget(StringID name,
		const MultiRenderTarget::CreateParams& params,
		float viewRelativeWidthFactor,
		float viewRelativeHeightFactor);
	virtual RenderTargetPtr CreateRenderTexture(StringID name,
		const RenderTexture::CreateParams& params,
		float viewRelativeWidthFactor,
		float viewRelativeHeightFactor);
	virtual RenderTargetPtr CreateRenderBuffer(StringID name,
		const RenderBuffer::CreateParams& params,
		float viewRelativeWidthFactor,
		float viewRelativeHeightFactor);
	virtual RenderPass* AddPass(const String& type);
	virtual void EndConfig();

	virtual uint32 GetRenderTargetCount() const;

	virtual void Commit(CommitContext& context);
		
	virtual RenderPass* FindPass(StringID name);
	virtual void EnablePass(StringID pass);
	virtual void DisablePass(StringID pass);

	virtual uint32 GetPassCount() const;
	virtual RenderPass* GetPass(uint32 i);


	void CreateResources();
	void DestroyResources();


protected:

	StringID GetName(const RenderTargetPtr&);
		
	static void DestroyResources(void* thisPointer);

	struct RenderRef {		
		bool useAsTarget;
		PixelFormat format;
		StringID ref;
	};

	struct MultiRenderTargetData : public AllocGeneral {
		uint16 numColorBuffer;
		RenderRef depth;
		RenderRef color[MultiRenderTarget::MAX_COLOR_TARGET];
	};

	struct BufferInfo {
		StringID name;
		RenderTargetType type;
		uint16 samples;
		float dx, dy;
		PixelFormat format;
		MultiRenderTargetData* mrtData;

		BufferInfo();
		BufferInfo(const BufferInfo& info);
		BufferInfo(BufferInfo&& info);
		~BufferInfo();

		BufferInfo& operator = (const BufferInfo& other);
		BufferInfo& operator = (BufferInfo&& other);
	};

	typedef vector<BufferInfo>::type BufferInfoList;

	struct MetaInfo : public AllocGeneral {
	public:

		MetaInfo();
		
		bool ValidateDimensions(uint32 width, uint32 height, uint32 depth);
		void AddBuffer(BufferInfo&& info);
	
		uint32 width, height, depth;
		BufferInfoList bufferInfo;
	};

	enum Flags {
		HAS_RELATIVE_TARGETS = 1 << 1,
	};

	struct BufferData {
		RenderTargetPtr target;
		float viewRelativeX;
		float viewRelativeY;
	};
	
	typedef map<StringID, BufferData>::type TargetMap;

	MetaInfo* metaInfo;
	Size currentSize;
	uint32 flags;
	TargetMap targets;
	RenderPassList renderPasses;

public:
	friend class DefaultStreamer;
		
	class DefaultStreamer : public Streamer {

		NEX_LOG_HELPER(DefaultStreamer);
	public:
		enum RenderScriptHeaders {
			FILE_MAGIC_RSCRIPT = 0xff817191,
			RSCRIPT_PASS_DATA = 0x55ee,
			RSCRIPT_BEGIN_PASS = 0x51fe,
			RSCRIPT_BUFFER = 0x50dd,
		};

		virtual bool Load(RenderSystem& s, InputStreamPtr& stream);
		virtual bool Save(RenderSystem& s, OutputStreamPtr& stream);

	protected:

		bool _ReadVersion(InputSerializer& stream);
		void _ReadPass(RenderSystem* s, ChunkInputStream& ostr);
		void _ReadBuffers(RenderSystem* s, InputSerializer& ostr);
		void _WriteBuffers(RenderSystem* s, const MetaInfo& m, OutputSerializer& ostr);
		void _WritePass(RenderSystem* s, RenderPass* pass, ChunkOutputStream& ostr);
		void _WriteVersion(OutputStreamPtr& stream);
	};

};

typedef RefPtr<RenderSystemImpl> RenderSystemImplPtr;
} /* namespace nextar */

#endif /* CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_ */

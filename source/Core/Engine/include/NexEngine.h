/*
 * NexEngine.h
 *
 *  Created on: 08-Mar-2014
 *      Author: obhi
 */

#ifndef NEXENGINE_H_
#define NEXENGINE_H_

#include <EngineConfig.h>
#include <BaseHeaders.h>

namespace nextar {

	class RenderManager;
	class RenderDriver;
	class RenderContext;
	class RenderWindow;
	class Buffer;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class Viewport;
	class BufferManager;
	class GpuProgram;
	class ShaderAsset;
	class TextureBase;
	class RenderTarget;
	class RenderTexture;
	class MultiRenderTarget;
	class CommitContext;
	class DebugDisplay;
	class Pass;
	class ContextObject;

	class Component;
	class SharedComponent;
	class ComponentFactoryArchive;
	class LightSystem;
	class BoundingVolume;
	class VisibilitySet;
	class VisiblePrimitive;

	class Entity;
	class Light;
	class Renderable;
	class Moveable;
	class Camera;
	class Spatial;
	class CullingSystem;

	class SceneAsset;

	class Asset;
	class AssetStreamRequest;
	class TextureAsset;
	class ShaderAsset;
	class MaterialAsset;
	class MeshAsset;

	class AssetStreamRequest;
	class AssetCallback;

	typedef RefPtr<LightSystem> LightSystemPtr;

	typedef RefPtr<VertexBuffer> VertexBufferPtr;
	typedef RefPtr<IndexBuffer> IndexBufferPtr;
	typedef RefPtr<ConstantBuffer> ConstantBufferPtr;
	typedef RefPtr<RenderContext> RenderContextPtr;
	typedef RefPtr<RenderDriver> RenderDriverPtr;
	typedef RefPtr<Buffer> BufferPtr;
	typedef RefPtr<RenderTexture> RenderTexturePtr;
	typedef RefPtr<RenderTarget> RenderTargetPtr;
	typedef RefPtr<MultiRenderTarget> MultiRenderTargetPtr;

	typedef RefPtr<SharedComponent> SharedComponentPtr;
	typedef RefPtr<Asset> AssetPtr;
	typedef RefPtr<TextureAsset> TextureAssetPtr;
	typedef RefPtr<ShaderAsset> ShaderAssetPtr;
	typedef RefPtr<MaterialAsset> MaterialAssetPtr;
	typedef RefPtr<MeshAsset> MeshAssetPtr;

	typedef RefPtr<Entity> EntityPtr;
	typedef RefPtr<SceneAsset> SceneAssetPtr;

	enum class EngineConstants : uint32 {
		NUM_ASSET_POINTER_PER_POOL_BLOCK = 128,
		NUM_STREAM_REQ_POINTER_PER_POOL_BLOCK = 32,
	};
}


#endif /* NEXENGINE_H_ */

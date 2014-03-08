#ifndef NEXTAR_NEXSYS_H__
#define NEXTAR_NEXSYS_H__

#include "NexTypes.h"
#include "Logger.h"

//#define NEX_MAX_STREAMS 8
//#define NEX_MAS_VERTEX_ELEMENTS 32

namespace nextar {

//typedef RefPtr<Material> MaterialPtr;

	namespace debug {
	}


	class FactoryPlugin;
	class PluginRegistry;
	class Config;
	class URL;
	class InputStream;
	class OutputStream;
	class Archive;
	class FileSystem;
	class FrameListener;
	class WindowManager;
	class LogManager;
	
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

	class Component;
	class SharedComponent;
	class ComponentFactoryArchive;
	class LightSystem;
	class BoundingVolume;
	class VisibilitySet;
	class VisiblePrimitive;

	class Frustum;

	class Entity;
	class Light;
	class Renderable;
	class Moveable;
	class Camera;
	class Spatial;
	class CullingSystem;

	class Scene;

	class Asset;
	class AssetStreamRequest;
	class TextureAsset;
	class ShaderAsset;
	class MaterialAsset;
	class MeshAsset;

	class AssetStreamRequest;
	struct AssetCallback;
	
	typedef RefPtr<LightSystem> LightSystemPtr;

	typedef RefPtr<VertexBuffer> VertexBufferPtr;
	typedef RefPtr<IndexBuffer> IndexBufferPtr;
	typedef RefPtr<ConstantBuffer> ConstantBufferPtr;
	typedef RefPtr<InputStream> InputStreamPtr;
	typedef RefPtr<OutputStream> OutputStreamPtr;
	typedef RefPtr<Archive> ArchivePtr;
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
	typedef RefPtr<Scene> ScenePtr;
}

#endif //NEXTAR_NEXSYS_H__


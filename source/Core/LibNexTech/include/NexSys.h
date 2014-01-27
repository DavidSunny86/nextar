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
	
	class RenAssetderManager;
	class RenderDriver;
	class RenderContext;
	class RenderWindow;
	class Buffer;
	class VertexBuffer;
	class IndexBuffer;
	class Viewport;
	class BufferManager;
	class GpuProgram;
	class Shader;
	class TextureBase;
	class RenderTarget;
	class RenderTexture;
	class MultiRenderTarget;
	class CommitContext;
	class DebugDisplay;
	class Pass;

	class Component;
	class ComponentManager;
	class ComponentManagerArchive;
	class LightSystem;
	class BoundingVolume;
	class VisibilitySet;
	class VisiblePrimitive;

	class Camera;

	class Node;
	class ContainerNode;
	class MeshNode;
	class LightNode;
	class RegionNode;

	class Asset;
	class Texture;
	class Shader;
	class Material;
	class Mesh;

	class AssetStreamRequest;
	class AssetCallback;
	class AssetManager;

	typedef RefPtr<LightSystem> LightSystemPtr;

	typedef RefPtr<VertexBuffer> VertexBufferPtr;
	typedef RefPtr<IndexBuffer> IndexBufferPtr;
	typedef RefPtr<InputStream> InputStreamPtr;
	typedef RefPtr<OutputStream> OutputStreamPtr;
	typedef RefPtr<Archive> ArchivePtr;
	typedef RefPtr<RenderWindow> RenderWindowPtr;
	typedef RefPtr<RenderContext> RenderContextPtr;
	typedef RefPtr<RenderDriver> RenderDriverPtr;
	typedef RefPtr<Buffer> BufferPtr;
	typedef RefPtr<RenderTexture> RenderTexturePtr;
	typedef RefPtr<MultiRenderTarget> MultiRenderTargetPtr;

	typedef RefPtr<Asset> AssetPtr;
	typedef RefPtr<Texture> TexturePtr;
	typedef RefPtr<Shader> ShaderPtr;
	typedef RefPtr<Material> MaterialPtr;
	typedef RefPtr<Mesh> MeshPtr;
}

#endif //NEXTAR_NEXSYS_H__


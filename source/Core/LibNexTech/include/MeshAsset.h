
#ifndef MESHASSET_H_
#define MESHASSET_H_

#include "NexSys.h"
#include "Asset.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "StreamData.h"
#include "MaterialAsset.h"

namespace nextar {

	enum {
		MAX_STREAM_COUNT = 8,
	};

	class MeshBufferData: public AllocGeneral {
	public:
		typedef list<ByteStream>::type BufferList;
		/* vertex/index buffer data when loaded from file */
		BufferList vertexBuffers;
		BufferList indexBuffers;

	};

	class MeshVertexData: public AllocGeneral {
	public:
		/* Num vertex buffs */
		uint16 numVertexBuffers;
		/* Num vertex element */
		uint16 numVertexElements;
		/* vertex count */
		uint32 vertexCount;
		/* element list, might be shared */
		VertexElement* vertexElements;
		/* Layout created for vertex data */
		VertexLayout* layout;
		/* This will be copied if the mesh has dynamic
		 per instance vertex buffer */
		VertexBufferBinding* binding;
		/* Stream list */
		VertexBufferPtr streams[MAX_STREAM_COUNT];

		inline MeshVertexData()
				: numVertexBuffers(0), vertexCount(0), numVertexElements(0),
				  vertexElements(nullptr), binding(nullptr), layout(nullptr) {
		}

		~MeshVertexData();

	};

	class MeshIndexData: public AllocGeneral {
	public:
		/* type */
		bool twoBytePerElement;
		/* index count */
		uint32 indexCount;
		/* index buffer */
		IndexBufferPtr ibdata;
	};

	class PrimitiveGroup: public AllocGeneral {
	public:

		BoundsInfo boundsInfo;
		/* Type, may vary depending upon pass */
		PrimitiveType primitiveType;
		/* Vertex data, buffers may be shared */
		MeshVertexData* vertexData;
		/* Index data, buffers may be shared */
		MeshIndexData* indexData;
		/* Material data */
		MaterialAssetPtr defaultMaterial;
		/* start vertex */
		uint32 startVertex;
		/* vertex count */
		uint32 vertexCount;
		/* index start */
		uint32 startIndex;
		/* index count */
		uint32 indexCount;
	};
	typedef vector<PrimitiveGroup>::type PrimitiveGroupList;


	class _NexExport MeshAssetManager: public AssetManager {
	protected:

	public:
		MeshAssetManager(const String& name);
		~MeshAssetManager();

		virtual MeshAssetPtr AsyncCreateInstance(const String& name, 
			const String& assetGroup, const URL& location);

		/* Implementation */
		virtual Component* AsyncCreateImpl(uint32 type, const String& name);
		virtual void AsyncDestroyImpl(Component*);
	};

	/**
	 * @class    Mesh
	 *
	 * @remarks  Base mesh class for all types of meshes
	 *
	 **/
	class _NexExport MeshAsset: public nextar::Asset {
		NEX_LOG_HELPER(MeshAsset);

	public:
		enum Type {
			CLASS_ID = Component::CLASS_ASSET_MESH,
			CATAGORY = COMPONENT_CAT(CLASS_ID),
		};
		class Loader;
		class LoaderImpl;

		class LoaderImpl {
		public:
			virtual void Load(InputStreamPtr&, MeshAsset::Loader&) = 0;
		};

		class Loader : public AllocGeneral {
			NEX_LOG_HELPER(MeshLoader);
			NEX_DECLARE_FACTORY(LoaderImpl);

		public:
			Loader(nextar::StreamRequest* req);
			~Loader();

			inline nextar::StreamRequest* GetRequestPtr() {
				return meshRequest;
			}

			static void EndianFlip(void* data, const VertexElement* veBegin,
					const VertexElement* veEnd, size_t count);

			void Serialize();

		protected:
			nextar::StreamRequest* meshRequest;
		};

		class StreamRequest : public AllocGeneral,
		public AssetStreamRequest {
		public:
			StreamRequest(MeshAsset* a);

			inline uint32 AddPrimitiveGroup() {
				uint32 size = (uint32)primitives.size();
				primitives.resize(size+1);
				return size;
			}

			inline void SetBoundsInfo(const BoundsInfo& bv) {
				boundsInfo = bv;
			}

			inline void SetPrimitiveGroupCount(uint32 count) {
				primitives.reserve(count);
			}

			inline void SetSharedVertexData(MeshVertexData* vd) {
				sharedVertexData = vd;
			}

			inline MeshVertexData* GetSharedVertexData() {
				return sharedVertexData;
			}

			inline void SetSharedIndexData(MeshIndexData* id) {
				sharedIndexData = id;
			}

			inline void SetSharedMaterial(MaterialAssetPtr& m) {
				sharedMaterial = m;
				if(sharedMaterial)
					metaInfo.AddDependency(sharedMaterial.GetPtr());
			}

			inline void SetPrimitiveVertexData(uint32 i, MeshVertexData* vertexData,
					uint32 startVertex, uint32 vertexCount) {
				PrimitiveGroup& pg = primitives[i];
				pg.vertexData = vertexData;
				pg.startVertex = startVertex;
				pg.vertexCount = vertexCount;
			}

			inline void SetPrimitiveIndexData(uint32 i, MeshIndexData* indexData,
					uint32 startIndex, uint32 indexCount) {
				PrimitiveGroup& pg = primitives[i];
				pg.indexData = indexData;
				pg.startIndex = startIndex;
				pg.indexCount = indexCount;
			}

			inline void SetPrimitiveMaterial(uint32 i, MaterialAssetPtr& m) {
				PrimitiveGroup& pg = primitives[i];
				pg.defaultMaterial = m;
				if(pg.defaultMaterial)
					metaInfo.AddDependency(pg.defaultMaterial);
			}

			inline void SetPrimitiveBoundsInfo(uint32 i, const BoundsInfo& bv) {
				PrimitiveGroup& pg = primitives[i];
				pg.boundsInfo = bv;
			}

			/* set buffer data */
			ByteStream& AddIndexBuffer();
			ByteStream& AddVertexBuffer();

		protected:

			friend class MeshAsset;

			MeshVertexData* sharedVertexData;
			MeshIndexData* sharedIndexData;
			MaterialAssetPtr sharedMaterial;

			BoundsInfo boundsInfo;
			PrimitiveGroupList primitives;
			MeshBufferData bufferData;
		};


		MeshAsset(MeshAssetManager*, const String&);
		~MeshAsset();
		
		const BoundsInfo& GetBoundsInfo() const {
			return boundsInfo;
		}

		uint16 GetNumVertexElements() const {
			return sharedVertexData ? sharedVertexData->numVertexElements : 0;
		}

		VertexElement* GetVertexElements() const {
			return sharedVertexData ? sharedVertexData->vertexElements : 0;
		}

		MeshIndexData* GetSharedIndexData() const {
			return sharedIndexData;
		}

		MeshVertexData* GetSharedVertexData() const {
			return sharedVertexData;
		}

		MaterialAssetPtr& GetSharedMaterial() {
			return defaultSharedMaterial;
		}

		PrimitiveGroup&  GetPrimitiveGroup(size_t i) {
			return primitives[i];
		}

		PrimitiveGroupList& GetPrimitiveGroups() {
			return primitives;
		}

		size_t GetPrimitiveGroupCount() const {
			return primitives.size();
		}

		/* Implementation */
		virtual uint32 GetClassID() const override;


	protected:

		virtual void NotifyAssetLoaded();
		virtual void NotifyAssetUnloaded();
		virtual void NotifyAssetUpdated();

		virtual void LoadImpl(nextar::StreamRequest* req, bool isAsync);
		virtual void UnloadImpl(nextar::StreamRequest* req, bool isAsync);

		virtual StreamRequest* CreateStreamRequestImpl(bool load);
		virtual void DestroyStreamRequestImpl(StreamRequest*&, bool load=true);

		static void _FillVertexData(MeshVertexData*, MeshBufferData::BufferList::iterator&);
		static void _FillIndexData(MeshIndexData*, MeshBufferData::BufferList::iterator&);

		BoundsInfo boundsInfo;
		bool vertexDeformationsEnabled;
		PrimitiveGroupList primitives;
		/* Default material shared by submeshes or NULL */
		MaterialAssetPtr defaultSharedMaterial;
		/* Shared vertex data, may be NULL */
		MeshVertexData* sharedVertexData;
		/* Shared index data, may be NULL */
		MeshIndexData* sharedIndexData;
	};

}

#endif //MESHASSET_H_

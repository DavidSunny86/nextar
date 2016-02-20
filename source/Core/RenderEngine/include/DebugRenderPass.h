/*
 * DebugRenderPass.h
 *
 *  Created on: 25-Oct-2014
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_
#define CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_

#include <NexRenderEngine.h>
#include <EngineHeaders.h>
#include <RenderPass.h>
#include <DebugDisplay.h>

namespace nextar {

class DebugPrimitive : public VisiblePrimitive {
public:
	DebugPrimitive(uint32 id, float timeToExpiry);
	~DebugPrimitive();
		
	uint32 GetPrimitiveID() const {
		return id;
	}

	inline void SetColor(const Color& c) {
		color = c;
	}

	inline const Color& GetColor() const {
		return color;
	}

	inline bool IsDead(float elapsed) {
		if (timeToDeath == 0.0f)
			return false;
		timeToDeath -= elapsed;
		if (timeToDeath <= 0)
			return true;
		return false;
	}

	inline void SetTransform(Vec3AF scale, QuatF rot, Vec3AF loc) {
		*worldMatrix = Mat4x4Mul(Mat4x4Mul(Mat4x4FromScale(scale), Mat4x4FromRot(rot)),
			Mat4x4FromPos(loc));
	}

	inline void SetTransform(Mat4x4R tform) {
		*worldMatrix = tform;
	}

	inline void SetConstantSize(float size) {
		constantSize = size;
	}

	virtual float GetConstantSize() const {
		return constantSize;
	}

	void SetDebugMaterial(MaterialAssetPtr& m);


protected:
	ParameterBuffer _parameters;
	Matrix4x4* worldMatrix;
	float constantSize;
	Color color;
	uint32 id;
	float timeToDeath;
};

class _NexRenderAPI DebugRenderPass:
	public DebugDisplay,
	public nextar::BaseRenderPass {

	NEX_LOG_HELPER(DebugRenderPass);

public:
		
	DebugRenderPass();
	virtual ~DebugRenderPass();

	virtual void Commit(CommitContext& context);

	virtual void CreateResources();
	virtual void DestroyResources();

	static RenderPass* CreateInstance();

protected:

	virtual VisiblePrimitiveList& _GetPrimitives(CommitContext& context);

	virtual uint32 _Register(AABoxF box, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(Mat4x4R tform, float screenSpaceFactor, const Color& color = Color::Black,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(PlaneF plane, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(const Frustum& frustum, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(const Geometry& triList, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(const Box2D& rect, const Color& color,
		Vec4AF textureOffsetAndRepeat,
		TextureBase* textured = 0, bool border = true,
		float expiryTimeInSec = 0.0f) override;

	virtual void _RemovePrimitive(uint32 id);


protected:

	void DetermineVisiblePrimitives(float frameTime);
	void CreateMaterials();
	void ReleaseObjects();
	void GenerateStreamDataForBox();
	void GenerateStreamDataForAxis();
	void GenerateStreamDataForQuad();

	MaterialAssetPtr debugQuadMaterial;
	MaterialAssetPtr debugMaterial;
	uint32 idCounter;
	bool boxDataGenerated;
	bool axisDataGenerated;
	bool quadDataGenerated;
	StreamData quadDataStream;
	StreamData boxDataStream;
	StreamData planeData;
	StreamData axisData;
	StreamData lineData;
	VisiblePrimitiveList alivePrimitives;
};

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_ */

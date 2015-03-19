/*
 * DebugRenderSystem.h
 *
 *  Created on: 25-Oct-2014
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_
#define CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_

#include <NexRenderEngine.h>
#include <EngineHeaders.h>
#include <RenderSystem.h>
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

protected:
	Matrix4x4* worldMatrix;
	Color color;
	uint32 id;
	float timeToDeath;
};

class DebugRenderSystem: 
	public DebugDisplay,
	public nextar::RenderSystem {

	NEX_LOG_HELPER(DebugRenderSystem);

public:
	DebugRenderSystem();
	virtual ~DebugRenderSystem();

	virtual void Commit(CommitContext& context);


	virtual VisiblePrimitiveList& GetPrimitives(CommitContext& context);

	virtual uint32 Register(const AABox3& box, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 Register(Mat4x4R tform, const Color& color = Color::Black,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 Register(PlaneF plane, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 Register(const Frustum& frustum, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 Register(const Geometry& triList, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 Register(const Box2D& rect, const Color& color,
		TextureBase* textured = 0, bool border = true,
		float expiryTimeInSec = 0.0f) override;

	virtual void RemovePrimitive(uint32 id);

	static void DestroyResources(void* renderSystem);
	static void CreateResources(void* renderSystem);

	
protected:

	void ClearStreamData(StreamData&);
	void DetermineVisiblePrimitives(float frameTime);
	void CreateMaterials();
	void ReleaseObjects();
	void GenerateStreamDataForBox();
	void GenerateStreamDataForAxis();

	MaterialAssetPtr debugQuadMaterial;
	MaterialAssetPtr debugMaterial;
	uint32 idCounter;
	bool boxDataGenerated;
	bool axisDataGenerated;
	StreamData boxDataStream;
	StreamData planeData;
	StreamData axisData;
	StreamData lineData;
	VisiblePrimitiveList alivePrimitives;
};

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_INCLUDE_DEBUGRENDERSYSTEM_H_ */

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
#include <BaseRenderPass.h>
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

	inline void SetTransform(Vec3A::pref scale, Quat::pref rot, Vec3A::pref loc) {
		*worldMatrix = Mat4x4Mul(Mat4x4Mul(Mat4x4FromScale(scale), Mat4x4FromRot(rot)),
			Mat4x4FromPos(loc));
	}

	inline void SetTransform(Mat4::pref tform) {
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
	Mat4::type* worldMatrix;
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
		
	typedef MetaType<DebugRenderPass>::Type MetaType;

	DebugRenderPass(const Meta* meta);
	virtual ~DebugRenderPass();

	virtual void Commit(CommitContext& context);

	virtual void CreateResources();
	virtual void DestroyResources();

protected:

	virtual VisiblePrimitiveList& _GetPrimitives(CommitContext& context);

	virtual uint32 _Register(AABox::pref box, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(Mat4::pref tform, float screenSpaceFactor, const Color& color = Color::Black,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(Plane::pref plane, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(const Frustum& frustum, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(const Geometry& triList, const Color& color,
		float expiryTimeInSec = 0.0f) override;
	virtual uint32 _Register(Rect::pref rect, const Color& color,
		Vec4::pref textureOffsetAndRepeat,
		TextureBase* textured, bool border,
		float expiryTimeInSec, MaterialAssetPtr material) override;

	virtual void _RemovePrimitive(uint32 id);


protected:

	static Geometry GenerateAxis(float radius, float alpha, int color);
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

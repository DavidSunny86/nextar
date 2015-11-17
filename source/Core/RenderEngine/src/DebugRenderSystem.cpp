/*
 * DebugRenderSystem.cpp
 *
 *  Created on: 25-Oct-2014
 *      Author: obhi
 */

#include <DebugRenderSystem.h>
#include <DebugDisplay.h>
#include <Geometry.h>

namespace nextar {

DebugPrimitive::DebugPrimitive(uint32 primId,
	float timeToExpiry) : id(primId), timeToDeath(timeToExpiry), color(Color::White), worldMatrix(NEX_NEW(Matrix4x4)), constantSize(0.0f) {
	this->worldMatrices = worldMatrix;
	*(worldMatrix) = Matrix4x4::IdentityMatrix;
	parameters = &_parameters;
}

DebugPrimitive::~DebugPrimitive() {
	NEX_DELETE(worldMatrix);
}

void DebugPrimitive::SetDebugMaterial(MaterialAssetPtr& m) {
	SetMaterial(m);
	ShaderAssetPtr s = m->GetShader();
	if (s) {
		parameters->Prepare(s->GetParamTableItem(ParameterContext::CTX_OBJECT));
	}
}

DebugRenderSystem::DebugRenderSystem() {
	idCounter = 0;
	boxDataGenerated = false;
	axisDataGenerated = false;
	quadDataGenerated = false;
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_INIT_RESOURCES, CreateResources, this);
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
}

DebugRenderSystem::~DebugRenderSystem() {
	ReleaseObjects();
}


void DebugRenderSystem::DestroyResources(void* renderSystem) {
	DebugRenderSystem* pRenderSys = reinterpret_cast<DebugRenderSystem*>(renderSystem);
	if (pRenderSys)
		pRenderSys->ReleaseObjects();
}


void DebugRenderSystem::CreateResources(void* renderSystem) {
	DebugRenderSystem* pRenderSys = reinterpret_cast<DebugRenderSystem*>(renderSystem);
	if (pRenderSys)
		pRenderSys->CreateMaterials();
}

void DebugRenderSystem::CreateMaterials() {
	URL debugMaterialPath(FileSystem::ArchiveEngineData_Name, "Materials/Assets/Debug3D.asset");
	URL debugQuadMaterialPath(FileSystem::ArchiveEngineData_Name, "Materials/Assets/DebugQuad.asset");
	try {
		debugMaterial = Asset::AssetLoad(debugMaterialPath);
	} catch (const GracefulErrorExcept& e) {
		Error(e.GetMsg());
	}

	try {
		debugQuadMaterial = Asset::AssetLoad(debugQuadMaterialPath);
	} catch (const GracefulErrorExcept& e) {
		Error(e.GetMsg());
	}

}

void DebugRenderSystem::ReleaseObjects() {

	std::for_each(alivePrimitives.begin(), alivePrimitives.end(),
		[] (VisiblePrimitive* o) {
		DebugPrimitive* db = static_cast<DebugPrimitive*>(o);
		NEX_DELETE(db);
	});
	alivePrimitives.clear();
	debugMaterial.Clear();
	debugQuadMaterial.Clear();
	ClearStreamData(boxDataStream);
	ClearStreamData(quadDataStream);
	ClearStreamData(axisData);
	boxDataGenerated = false;
}

void DebugRenderSystem::ClearStreamData(StreamData& s) {
	s.indices.Clear();
	if (s.vertices.binding) {
		NEX_DELETE(s.vertices.binding);
		s.vertices.binding = nullptr;
	}
}

VisiblePrimitiveList& DebugRenderSystem::GetPrimitives(CommitContext& context) {
	return alivePrimitives;
}

uint32 DebugRenderSystem::Register(AABoxF box,
	const Color& color, float expiryTimeInSec) {
	if (!boxDataGenerated) {
		GenerateStreamDataForBox();
	}
	DebugPrimitive* primitive = nullptr;

	primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	primitive->SetStreamData(&boxDataStream);
	primitive->SetDebugMaterial(debugMaterial);
	primitive->SetColor(color);
	primitive->SetTransform(AABoxGetSize(box), QuatIdentity(), AABoxGetCenter(box));
	alivePrimitives.push_back(primitive);

	return idCounter;
}

uint32 DebugRenderSystem::Register(Mat4x4R tform,
	float screenSpaceFactor,
	const Color& color, float expiryTimeInSec) {
	if (!axisDataGenerated) {
		GenerateStreamDataForAxis();
	}
	DebugPrimitive* primitive = nullptr;
	primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	primitive->SetStreamData(&axisData);
	primitive->SetDebugMaterial(debugMaterial);
	primitive->SetColor(color);
	primitive->SetTransform(tform);
	primitive->SetConstantSize(screenSpaceFactor);
	alivePrimitives.push_back(primitive);

	return idCounter;
}

uint32 DebugRenderSystem::Register(PlaneF plane,
	const Color& color, float expiryTimeInSec) {
	//DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	//alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderSystem::Register(const Frustum& frustum,
	const Color& color, float expiryTimeInSec) {
	//DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	//alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderSystem::Register(const Box2D& rect,
	const Color& color, Vec4AF textureOffsetAndRepeat, TextureBase* textured, bool border,
	float expiryTimeInSec) {
	// remap rect
	Box2D remapped(rect);
	remapped.min -= Vector2(0.5f, 0.5f);
	remapped.max -= Vector2(0.5f, 0.5f);
	remapped.min *= 2;
	remapped.max *= 2;
	
	// actual size is 2,2
	Vector2 size = remapped.GetSize() * 0.5;
	Vector2 center = remapped.GetCenter();
	if (!quadDataGenerated) {
		GenerateStreamDataForQuad();
		quadDataGenerated = true;
	}

	DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	primitive->SetDebugMaterial(debugQuadMaterial);
	primitive->SetStreamData(&quadDataStream);
	ParameterBuffer* b = primitive->GetParameters();
	if (!textured)
		textured = RenderManager::Instance().GetDefaultTexture();
	Vector4A translationAndScale[3] = {
		Vec4ASet(	center.x, center.y, size.x, size.y	),
		textureOffsetAndRepeat,
		Vec4ASet(color.red, color.green, color.blue, color.alpha)
	};
	TextureUnit textureData;
	textureData.texture = textured;
	b->SetData(translationAndScale, 0, sizeof(Vector4A)*3);
	b->SetData(&textureData, sizeof(Vector4A)*3);
	alivePrimitives.push_back(primitive);
	
	return idCounter;
}

uint32 DebugRenderSystem::Register(const Geometry& triList, const Color& color,
	float expiryTimeInSec) {
	//DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	//alivePrimitives.push_back(primitive);
	return idCounter;
}

void DebugRenderSystem::DetermineVisiblePrimitives(float frameTime) {

	for (uint32 i = 0; i < alivePrimitives.size(); ) {
		auto p = alivePrimitives[i];
		DebugPrimitive* dp = static_cast<DebugPrimitive*>(p);
		if (dp->IsDead(frameTime)) {

			if (alivePrimitives.size() > 1) {
				alivePrimitives[i] = *alivePrimitives.rbegin();
				alivePrimitives.pop_back();
			} else
				alivePrimitives.clear();

			NEX_DELETE(dp);
		} else
			++i;
	}
}

void DebugRenderSystem::RemovePrimitive(uint32 id) {
	for (auto it = alivePrimitives.begin(); it != alivePrimitives.end(); ++it) {
		DebugPrimitive* dp = static_cast<DebugPrimitive*>(*it);
		if (dp->GetPrimitiveID() == id) {
			alivePrimitives.erase(it);
			NEX_DELETE(dp);
		}
	}
}

void DebugRenderSystem::Commit(CommitContext& context) {

	DetermineVisiblePrimitives(context.frameTime);
	// @urgent We should not clear here
	context.renderTargetInfo.info.clearColor[0] = Color::Black;
	context.renderTargetInfo.info.clearFlags = ClearFlags::CLEAR_COLOR;
	context.renderContext->BeginRender(&context.renderTargetInfo);
	for (auto &prim : alivePrimitives) {
		MaterialAsset* material = prim->GetMaterial();
		ShaderAsset* shader = material->GetShader();
		DebugPrimitive* dp = static_cast<DebugPrimitive*>(prim);
		if (context.shader != shader) {
			context.shader = shader;
			// deferred pass at 0
			Pass& pass = context.shader->GetPass(0);
			context.passNumber = pass.GetID();
			context.pass = static_cast<Pass::View*>(context.renderContext->GetView(&pass));
			context.paramBuffers[(uint32)ParameterContext::CTX_PASS] = &shader->GetParameters();
			context.pass->SwitchAndUpdateParams(context);
		}
		if (context.material != material) {
			context.material = material;
			context.materialNumber++;
			context.paramBuffers[(uint32)ParameterContext::CTX_MATERIAL] = context.material->GetParameters();
			context.pass->UpdateParams(context, ParameterContext::CTX_MATERIAL, context.materialNumber);
		}

		context.primitive = prim;
		context.color = dp->GetColor();
		context.paramBuffers[(uint32)ParameterContext::CTX_OBJECT] = prim->GetParameters();
		context.pass->UpdateParams(context, ParameterContext::CTX_OBJECT, (uint32)(ptrdiff_t)prim + context.frameNumber);
		context.renderContext->Draw(prim->GetStreamData(), context);
	}
	context.renderContext->EndRender();
}

Geometry GenerateAxis(float radius, float alpha, int color) {
	Quaternion q;
	Matrix4x4 m;
	Color baseColor(alpha, 0.1f, 0.1f, 0.1f);
	Color fullColor(alpha, 0.1f, 0.1f, 0.1f);
	Color halfColor(alpha, 0.1f, 0.1f, 0.1f);
	baseColor[color] = 0.8f;
	fullColor[color] = 1;
	halfColor[color] = 0.6f;

	Geometry cone = Geometry::CreateCone(10, radius, 0, radius * 6, false, true, baseColor, fullColor);
	Geometry coneHandle = Geometry::CreateCone(4, radius*.4f, radius*0.01f, radius * 20, false, true, halfColor, baseColor);

	q = QuatFromAxisAng(Vec3ASet(0, 0, 1), Math::PI);
	m = Mat4x4FromScaleRotPos(1, q, Vec3ASet(0, -radius*0.2f, 0));
	coneHandle.Transform(m);
	cone.Merge(coneHandle);
	m = Mat4x4FromPos(Vec3ASet(0, radius * 21.2f, 0));
	cone.Transform(m);
	return cone;
}

void DebugRenderSystem::GenerateStreamDataForAxis() {

	float alpha = 0.7f;
	float radius = 2.0f;
	Quaternion q;
	Matrix4x4 m;

	Geometry yAxis = GenerateAxis(radius, alpha, 1);
	Geometry xAxis = GenerateAxis(radius, alpha, 0);
	q = QuatFromAxisAng(Vec3ASet(0, 0, 1), -Math::PI_BY_2);
	m = Mat4x4FromRot(q);
	xAxis.Transform(m);
	Geometry zAxis = GenerateAxis(radius, alpha, 2);
	q = QuatFromAxisAng(Vec3ASet(1, 0, 0), Math::PI_BY_2);
	m = Mat4x4FromRot(q);
	zAxis.Transform(m);

	Geometry main = Geometry::CreateSphere(10, radius/1.5f, false, true, Color(alpha, 0.7f, 0.7f, 0.7f));

	//cone2.Transform(m);
	main.Merge(xAxis);
	main.Merge(yAxis);
	main.Merge(zAxis);
	//cone.Merge(cone2);

	VertexBufferPtr vertexBuffer = VertexBuffer::Create(GpuBuffer::NEVER_RELEASED);

	uint32 stride = ((sizeof(float) * 3) + sizeof(uint32));
	uint32 vbsize = (uint32)main.points.size() * stride;

	void* pVData = NEX_ALLOC(vbsize, MEMCAT_GENERAL);
	float* pos = (float*)pVData;
	for (uint32 i = 0; i < (uint32)main.points.size(); ++i) {
		auto &p = main.points[i];
		auto& c = main.colors[i];

		*pos++ = p.x;
		*pos++ = p.y;
		*pos++ = p.z;
		*((uint32*)(pos++)) = c.ToAbgr();
	}

	vertexBuffer->CreateBuffer(vbsize, stride, reinterpret_cast<const uint8*>(pVData));

	IndexBufferPtr indexBuffer = Assign(NEX_NEW(IndexBuffer(GpuBuffer::NEVER_RELEASED)));
	indexBuffer->CreateBuffer(main.topology.size() * 2, IndexBuffer::Type::TYPE_16BIT,
		reinterpret_cast<const uint8*>(main.topology.data()));

	StreamData* stream = &axisData;
	stream->flags = StreamData::DELETE_BINDING;
	stream->type = PrimitiveType::PT_TRI_LIST;
	stream->vertices.count = (uint32)main.points.size();
	stream->vertices.start = 0;
	stream->vertices.layout = VertexLayout::GetCommonLayout(VertexLayoutType::POSITION_COLOR_0).GetPtr();
	stream->vertices.binding = NEX_NEW(VertexBufferBinding());
	stream->vertices.binding->SetBufferCount(1);
	stream->vertices.binding->BindBuffer(0, vertexBuffer, 0);
	stream->indices.start = 0;
	stream->indices.count = (uint32)main.topology.size();
	stream->indices.indices = std::move(indexBuffer);
	stream->instanceCount = 1;



	NEX_FREE(pVData, MEMCAT_GENERAL);
}

void DebugRenderSystem::GenerateStreamDataForBox() {
	Geometry boxData = Geometry::CreateBox(1, 1, 1, true, Color::White);
	VertexBufferPtr vertexBuffer = Assign(NEX_NEW(VertexBuffer(GpuBuffer::NEVER_RELEASED)));

	uint32 stride = ((sizeof(float) * 3) + sizeof(uint32));
	uint32 vbsize = (uint32)boxData.points.size() * stride;

	void* pVData = NEX_ALLOC(vbsize, MEMCAT_GENERAL);
	float* pos = (float*)pVData;
	for (uint32 i = 0; i < (uint32)boxData.points.size(); ++i) {
		auto &p = boxData.points[i];
		auto& c = boxData.colors[i];

		*pos++ = p.x;
		*pos++ = p.y;
		*pos++ = p.z;
		*((uint32*)(pos++)) = c.ToAbgr();
	}

	vertexBuffer->CreateBuffer(vbsize, stride, reinterpret_cast<const uint8*>(pVData));

	IndexBufferPtr indexBuffer = Assign(NEX_NEW(IndexBuffer(GpuBuffer::NEVER_RELEASED)));
	indexBuffer->CreateBuffer(boxData.topology.size()*2, IndexBuffer::Type::TYPE_16BIT,
		reinterpret_cast<const uint8*>(boxData.topology.data()));

	StreamData* stream = &boxDataStream;
	stream->flags = StreamData::DELETE_BINDING;
	stream->type = PrimitiveType::PT_LINE_LIST;
	stream->vertices.count = (uint32)boxData.points.size();
	stream->vertices.start = 0;
	stream->vertices.layout = VertexLayout::GetCommonLayout(VertexLayoutType::POSITION_COLOR_0).GetPtr();
	stream->vertices.binding = NEX_NEW(VertexBufferBinding());
	stream->vertices.binding->SetBufferCount(1);
	stream->vertices.binding->BindBuffer(0, vertexBuffer, 0);
	stream->indices.start = 0;
	stream->indices.count = (uint32)boxData.topology.size();
	stream->indices.indices = std::move(indexBuffer);
	stream->instanceCount = 1;

	NEX_FREE(pVData, MEMCAT_GENERAL);
}

void DebugRenderSystem::GenerateStreamDataForQuad() {
	Geometry quadData = Geometry::CreateQuad(1, 1);
	VertexBufferPtr vertexBuffer = Assign(NEX_NEW(VertexBuffer(GpuBuffer::NEVER_RELEASED)));

	uint32 stride = ((sizeof(float) * 2));
	uint32 vbsize = (uint32)quadData.points.size() * stride;

	void* pVData = NEX_ALLOC(vbsize, MEMCAT_GENERAL);
	float* pos = (float*)pVData;
	for (uint32 i = 0; i < (uint32)quadData.points.size(); ++i) {
		auto &p = quadData.points[i];
		
		*pos++ = p.x;
		*pos++ = p.y;
	}

	vertexBuffer->CreateBuffer(vbsize, stride, reinterpret_cast<const uint8*>(pVData));

	StreamData* stream = &quadDataStream;
	stream->flags = StreamData::DELETE_BINDING;
	stream->type = PrimitiveType::PT_TRI_LIST;
	stream->vertices.count = (uint32)quadData.points.size();
	stream->vertices.start = 0;
	stream->vertices.layout = VertexLayout::GetCommonLayout(VertexLayoutType::POSITION2D_0).GetPtr();
	stream->vertices.binding = NEX_NEW(VertexBufferBinding());
	stream->vertices.binding->SetBufferCount(1);
	stream->vertices.binding->BindBuffer(0, vertexBuffer, 0);
	stream->indices.start = 0;
	stream->indices.count = 0;
	stream->instanceCount = 1;

	NEX_FREE(pVData, MEMCAT_GENERAL);
}

} /* namespace nextar */

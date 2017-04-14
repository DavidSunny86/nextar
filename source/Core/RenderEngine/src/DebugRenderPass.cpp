/*
 * DebugRenderPass.cpp
 *
 *  Created on: 25-Oct-2014
 *      Author: obhi
 */

#include <DebugRenderPass.h>
#include <DebugDisplay.h>
#include <Geometry.h>

namespace nextar {

DebugPrimitive::DebugPrimitive(uint32 primId,
	float timeToExpiry) : id(primId), timeToDeath(timeToExpiry), color(Color::White), worldMatrix(NEX_NEW(Mat4::type)), constantSize(0.0f) {
	this->worldMatrices = worldMatrix;
	*(worldMatrix) = Mat4::type::IdentityMatrix;
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

DebugRenderPass::DebugRenderPass(const Meta* meta) : BaseRenderPass(meta) {

	idCounter = 0;
	boxDataGenerated = false;
	axisDataGenerated = false;
	quadDataGenerated = false;
}

DebugRenderPass::~DebugRenderPass() {
	ReleaseObjects();
}

void DebugRenderPass::DestroyResources() {
	ReleaseObjects();
}


void DebugRenderPass::CreateResources() {
	CreateMaterials();
}

void DebugRenderPass::CreateMaterials() {
	URL debugMaterialPath(FileSystem::ArchiveEngineData_Name, "Materials/Assets/Debug3D.asset");
	URL debugQuadMaterialPath(FileSystem::ArchiveEngineData_Name, "Materials/Assets/DrawQuad.asset");
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

void DebugRenderPass::ReleaseObjects() {

	std::for_each(alivePrimitives.begin(), alivePrimitives.end(),
		[] (VisiblePrimitive* o) {
		DebugPrimitive* db = static_cast<DebugPrimitive*>(o);
		NEX_DELETE(db);
	});
	alivePrimitives.clear();
	debugMaterial.Clear();
	debugQuadMaterial.Clear();
	boxDataStream.Clear();
	quadDataStream.Clear();
	axisData.Clear();
	quadDataGenerated = false;
	axisDataGenerated = false;
	boxDataGenerated = false;
}

VisiblePrimitiveList& DebugRenderPass::_GetPrimitives(CommitContext& context) {
	return alivePrimitives;
}

uint32 DebugRenderPass::_Register(AABox::pref box,
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

uint32 DebugRenderPass::_Register(Mat4::pref tform,
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

uint32 DebugRenderPass::_Register(Plane::pref plane,
	const Color& color, float expiryTimeInSec) {
	//DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	//alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderPass::_Register(const Frustum& frustum,
	const Color& color, float expiryTimeInSec) {
	//DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	//alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderPass::_Register(Rect::pref rect,
	const Color& color, Vec4::pref textureOffsetAndRepeat, TextureBase* textured, bool border,
	float expiryTimeInSec,
	MaterialAssetPtr material) {
	// remap rect
	Rect::type remapped;
	remapped.min = Vec2::Mul(Vec2::Sub(rect.min, Vec2::type{ 0.5f, 0.5f }), 2.0f);
	remapped.max = Vec2::Mul(Vec2::Sub(rect.max, Vec2::type{ 0.5f, 0.5f }), 2.0f);
	
	// actual size is 2,2
	Vec2::type size = Rect::GetHalfSize(remapped);
	Vec2::type center = Rect::GetCenter(remapped);
	if (!quadDataGenerated) {
		GenerateStreamDataForQuad();
		quadDataGenerated = true;
	}

	DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	primitive->SetDebugMaterial(material ? material : debugQuadMaterial);
	primitive->SetStreamData(&quadDataStream);
	ParameterBuffer* b = primitive->GetParameters();
	if (!textured)
		textured = RenderManager::Instance().GetDefaultTexture();
	Vec4::type translationAndScale[3] = {
		Vec4::Set(	center.x, center.y, size.x, size.y	),
		textureOffsetAndRepeat,
		Vec4::Set(color.red, color.green, color.blue, color.alpha)
	};
	TextureUnit textureData;
	textureData.texture = textured;
	b->SetData(translationAndScale, 0, sizeof(Vec4::type)*3);
	b->SetData(&textureData, sizeof(Vec4::type)*3);
	alivePrimitives.push_back(primitive);
	
	return idCounter;
}

uint32 DebugRenderPass::_Register(const Geometry& triList, const Color& color,
	float expiryTimeInSec) {
	//DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	//alivePrimitives.push_back(primitive);
	return idCounter;
}

void DebugRenderPass::DetermineVisiblePrimitives(float frameTime) {

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

void DebugRenderPass::_RemovePrimitive(uint32 id) {
	for (auto it = alivePrimitives.begin(); it != alivePrimitives.end(); ++it) {
		DebugPrimitive* dp = static_cast<DebugPrimitive*>(*it);
		if (dp->GetPrimitiveID() == id) {
			alivePrimitives.erase(it);
			NEX_DELETE(dp);
		}
	}
}

void DebugRenderPass::Commit(CommitContext& context) {
	BeginRender(context);
	for (auto &prim : alivePrimitives) {
		DebugPrimitive* dp = static_cast<DebugPrimitive*>(prim);
		context.color = dp->GetColor();
		RenderPrimitive(context, (uint32)(ptrdiff_t)prim, dp);
	}
	EndRender(context);
	DetermineVisiblePrimitives(context.frameTime);
}

Geometry DebugRenderPass::GenerateAxis(float radius, float alpha, int color) {
	Quat::type q;
	Mat4::type m;
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

void DebugRenderPass::GenerateStreamDataForAxis() {

	float alpha = 0.7f;
	float radius = 2.0f;
	Quat::type q;
	Mat4::type m;

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
	indexBuffer->CreateBuffer(static_cast<uint32>(main.topology.size()) << 1, IndexBuffer::Type::TYPE_16BIT,
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

void DebugRenderPass::GenerateStreamDataForBox() {
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
	indexBuffer->CreateBuffer(static_cast<uint32>(boxData.topology.size()) << 1, IndexBuffer::Type::TYPE_16BIT,
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

void DebugRenderPass::GenerateStreamDataForQuad() {
	quadDataStream = RenderManager::Instance().GetFullScreenQuad();
	quadDataStream.flags &=~StreamData::DELETE_BINDING;
}

} /* namespace nextar */

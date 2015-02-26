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
	float timeToExpiry) : id(primId), timeToDeath(timeToExpiry), color(Color::White), worldMatrix(NEX_NEW(Matrix4x4)) {
	this->worldMatrices = worldMatrix;
	*(worldMatrix) = Matrix4x4::IdentityMatrix;
}

DebugPrimitive::~DebugPrimitive() {
	NEX_DELETE(worldMatrix);
}

DebugRenderSystem::DebugRenderSystem() {
	idCounter = 0;
	boxDataGenerated = false;
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
	URL debugMaterialPath(FileSystem::ArchiveEngineData_Name, "Materials/Assets/Debug3D.mtl");
	debugMaterial = Asset::AssetLoad(debugMaterialPath);
}

void DebugRenderSystem::ReleaseObjects() {

	std::for_each(alivePrimitives.begin(), alivePrimitives.end(), 
		[] (VisiblePrimitive* o) {
		DebugPrimitive* db = static_cast<DebugPrimitive*>(o);
		NEX_DELETE(db);
	});
	alivePrimitives.clear();
	debugMaterial.Clear();
	ClearStreamData(boxDataStream);
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

uint32 DebugRenderSystem::Register(const AABox3& box,
	const Color& color, float expiryTimeInSec) {
	if (!boxDataGenerated) {
		GenerateStreamDataForBox();
	}

	DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	primitive->SetStreamData(&boxDataStream);
	primitive->SetMaterial(debugMaterial);
	primitive->SetTransform(box.GetSize(), QuatIdentity(), box.GetCenter());
	alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderSystem::Register(Mat3x4R tform,
	const Color& color, float expiryTimeInSec) {
	DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderSystem::Register(PlaneF plane,
	const Color& color, float expiryTimeInSec) {
	DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderSystem::Register(const Frustum& frustum,
	const Color& color, float expiryTimeInSec) {
	DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderSystem::Register(const Box2D& rect,
	const Color& color, TextureBase* textured, bool border,
	float expiryTimeInSec) {
	//DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	//alivePrimitives.push_back(primitive);
	return idCounter;
}

uint32 DebugRenderSystem::Register(const Geometry& triList, const Color& color,
	float expiryTimeInSec) {
	DebugPrimitive* primitive = NEX_NEW(DebugPrimitive(++idCounter, expiryTimeInSec));
	alivePrimitives.push_back(primitive);
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
		context.pass->UpdateParams(context, ParameterContext::CTX_OBJECT, (uint32)prim);
		context.renderContext->Draw(prim->GetStreamData(), context);
	}
	context.renderContext->EndRender();
}

void DebugRenderSystem::GenerateStreamDataForBox() {
	Geometry boxData = Geometry::CreateBox(1, 1, 1, true, Color::Red);
	VertexBuffer vertexBuffer(GpuBuffer::NEVER_RELEASED);
	
	uint32 stride = ((sizeof(float) * 3) + sizeof(uint32));
	uint32 vbsize = (uint32)boxData.points.size() * stride;

	void* pVData = NEX_ALLOC(vbsize, MEMCAT_GENERAL);
	float* pos = (float*)pVData;
	for (uint32 i = 0; i < boxData.points.size(); ++i) {
		auto &p = boxData.points[i];
		auto& c = boxData.colors[i];
		
		*pos++ = p.x;
		*pos++ = p.y;
		*pos++ = p.z;
		*((uint32*)(pos++)) = c.ToRgba();
	}

	vertexBuffer.CreateBuffer(vbsize, stride, reinterpret_cast<const uint8*>(pVData));

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
	stream->vertices.binding->BindBuffer(0, std::move(vertexBuffer));
	stream->indices.start = 0;
	stream->indices.count = (uint32)boxData.topology.size();
	stream->indices.indices = std::move(indexBuffer);
	stream->instanceCount = 1;

	NEX_FREE(pVData, MEMCAT_GENERAL);
}

} /* namespace nextar */

/*
 * MeshServices.cpp
 *
 *  Created on: 15-Jul-2014
 *      Author: obhi
 */

#include <MeshServices.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(MeshServices);
MeshServices::MeshServices() :
		unitSpotLV(nullptr)
		,unitOmniLV(nullptr) {
}

MeshServices::~MeshServices() {
	Close();
}

void MeshServices::SafeDelete(StreamData*& what) {
	if (what) {
		what->indices.indices.Clear();
		NEX_DELETE(what);
		what = nullptr;
	}
}

void MeshServices::Close() {
	SafeDelete(unitOmniLV);
	SafeDelete(unitSpotLV);
}

void MeshServices::Configure(const Config& config) {

}

StreamData* MeshServices::CreateSphereMesh(
		uint32 density,
		float radius,
		bool normalData,
		bool uvData) {

	// @todo Remove and use GeometryServices
	// @optimize Its probably better to use a single vertex buffer for all volumes
	uint32 width = density;
	uint32 height = density;
	uint32 nvec = (height-2)* width+2;
	uint32 ntri = (height-2)*(width-1)*2;
	uint32 vbsize = nvec * 3*sizeof(float);
	uint32 ibsize = ntri * 3*sizeof(uint16);

	float* dat = (float*)NEX_ALLOC(vbsize, MEMCAT_GENERAL);
	uint32 t=0;
	for(uint32 j=1; j<height-1; j++) {
		float theta = float(j)/(height-1) * Math::PI;
		float sinTheta, cosTheta;
		Math::SinCos(theta, sinTheta, cosTheta);
		for(uint32 i=0; i<width; i++ ) {
			float phi   = float(i)/(width-1 ) * Math::TWO_PI;
			float sinPhi, cosPhi;
			Math::SinCos(phi, sinPhi, cosPhi);
		    dat[t++] =  radius*sinTheta * cosPhi;
		    dat[t++] =  radius*cosTheta;
		    dat[t++] = -radius*sinTheta * sinPhi;
		}
	}
	dat[t++]=0; dat[t++]= radius; dat[t++]=0;
	dat[t++]=0; dat[t++]=-radius; dat[t++]=0;

	t = 0;
	uint16* idx = (uint16*)NEX_ALLOC(ibsize, MEMCAT_GENERAL);
	for(uint32 j=0; j<height-3; j++ ) {
	  for(uint32 i=0; i<width-1; i++ ) {
	    idx[t++] = (uint16) (j  )*width + i  ;
	    idx[t++] = (uint16) (j+1)*width + i+1;
	    idx[t++] = (uint16) (j  )*width + i+1;
	    idx[t++] = (uint16) (j  )*width + i  ;
	    idx[t++] = (uint16) (j+1)*width + i  ;
	    idx[t++] = (uint16) (j+1)*width + i+1;
	  }
	}
	for(uint32 i=0; i<width-1; i++ ) {
		idx[t++] = (uint16) (height-2)*width;
		idx[t++] = (uint16) i;
		idx[t++] = (uint16) i+1;
		idx[t++] = (uint16) (height-2)*width+1;
		idx[t++] = (uint16) (height-3)*width + i+1;
		idx[t++] = (uint16) (height-3)*width + i;
	}


	VertexBufferPtr vertexBuffer = VertexBuffer::Create(GpuBuffer::NEVER_RELEASED);
	IndexBufferPtr indexBuffer = Assign(NEX_NEW(IndexBuffer(GpuBuffer::NEVER_RELEASED)));
	vertexBuffer->CreateBuffer(vbsize, sizeof(float)* 3, reinterpret_cast<const uint8*>(dat));
	indexBuffer->CreateBuffer(ibsize, IndexBuffer::Type::TYPE_16BIT, reinterpret_cast<const uint8*>(idx));

	StreamData* stream = NEX_NEW(StreamData());
	stream->flags = StreamData::DELETE_BINDING;
	stream->vertices.count = nvec;
	stream->vertices.start = 0;
	stream->vertices.layout = VertexLayout::GetCommonLayout(VertexLayoutType::POSITION_0).GetPtr();
	stream->vertices.binding = NEX_NEW(VertexBufferBinding());
	stream->vertices.binding->SetBufferCount(1);
	stream->vertices.binding->BindBuffer(0, vertexBuffer, 0);
	stream->indices.start = 0;
	stream->indices.count = ntri*3;
	stream->indices.indices = std::move(indexBuffer);

	NEX_FREE(dat, MEMCAT_GENERAL);
	NEX_FREE(idx, MEMCAT_GENERAL);
	return stream;
}

StreamData* MeshServices::GetUnitOmniVolume() {
	if (!unitOmniLV) {
		unitOmniLV = CreateSphereMesh(32, 1, false, false);
	}
	return unitOmniLV;
}

StreamData* MeshServices::GetUnitSpotVolume() {
	if (!unitSpotLV) {
		// @todo
		unitSpotLV = CreateSphereMesh(32, 1, false, false);
	}
	return unitSpotLV;
}

} /* namespace nextar */

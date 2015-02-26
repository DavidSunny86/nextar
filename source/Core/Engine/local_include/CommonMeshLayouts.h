#ifndef COMMONMESHLAYOUTS_H_
#define COMMONMESHLAYOUTS_H_

namespace nextar {

const VertexElement Layout_POSITION_0[] = {
	VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
};

const VertexElement Layout_POSITION_NORMAL_0[] = {
	VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
	VertexElement(VertexDesc( VertexSemantic(COMP_NORMAL, 0, COMP_TYPE_FLOAT3), 12 ), 0, 0)
};

const VertexElement Layout_POSITION_NORMAL_UV_0[] = {
	VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
	VertexElement(VertexDesc( VertexSemantic(COMP_NORMAL, 0, COMP_TYPE_FLOAT3), 12 ), 0, 0),
	VertexElement(VertexDesc( VertexSemantic(COMP_TEXTURE_COORDINATE, 0, COMP_TYPE_FLOAT2), 12 ), 0, 0)
};

const VertexElement Layout_POSITION_0_NORMAL_1[] = {
	VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
	VertexElement(VertexDesc( VertexSemantic(COMP_NORMAL, 0, COMP_TYPE_FLOAT3), 12 ), 1, 0)
};

const VertexElement Layout_POSITION_0_NORMAL_UV_1[] = {
	VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
	VertexElement(VertexDesc( VertexSemantic(COMP_NORMAL, 0, COMP_TYPE_FLOAT3), 12 ), 1, 0),
	VertexElement(VertexDesc( VertexSemantic(COMP_TEXTURE_COORDINATE, 0, COMP_TYPE_FLOAT2), 12 ), 1, 0)
};

const VertexElement Layout_POSITION_COLOR_0[] = {
	VertexElement(VertexDesc(VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0), 0, 0),
	VertexElement(VertexDesc(VertexSemantic(COMP_COLOR, 0, COMP_TYPE_COLOR), 12), 0, 0)
};

VertexLayout::CommonVertexElement
VertexLayout::commonElementLayout[VertexLayoutType::VERTEX_LAYOUT_COUNT] = {
	{
		1,
		Layout_POSITION_0
	},
	{
		2,
		Layout_POSITION_NORMAL_0
	},
	{
		3,
		Layout_POSITION_NORMAL_UV_0
	},
	{
		2,
		Layout_POSITION_0_NORMAL_1
	},
	{
		3,
		Layout_POSITION_0_NORMAL_UV_1
	},
	{
		2,
		Layout_POSITION_COLOR_0
	}
};

VertexLayoutPtr VertexLayout::commonLayouts[VertexLayoutType::VERTEX_LAYOUT_COUNT];

}

#endif //COMMONMESHLAYOUTS_H_

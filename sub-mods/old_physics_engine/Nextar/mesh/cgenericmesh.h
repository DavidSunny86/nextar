//@ Abhishek Dey
//@ Nov 22 2007

#ifndef NEXTAR_CGENERICMESH_H
#define NEXTAR_CGENERICMESH_H

namespace nextar
{

class cGenericMesh : public iMesh
{
	cVertexBuffer*	mpkVertices;
	cIndexBuffer*	mpkIndices;
public:
	virtual void	setLod(f32) {}
};

}

#endif //NEXTAR_CGENERICMESH_H
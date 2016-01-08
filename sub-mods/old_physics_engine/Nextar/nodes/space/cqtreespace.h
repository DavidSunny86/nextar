//@ Abhishek Dey
//@ Oct 24 2007

#ifndef NEXTAR_QTREECOLLISION_SPACE_H
#define NEXTAR_QTREECOLLISION_SPACE_H

//@ based on a previous code i wrote about it
#include <xmath_c.h>
#include <ispace.h>
#include <icollisionspace.h>
#include <list_t.h>
namespace nextar
{

//@ quad tree space is a spatial ds and encapsulates the
//@  functionality of conceptual space, deriving from the
//@  fact that a spatial ds can be a logical ds but not 
//@  necessarily vice versa. 
class iEntity;
class cQTreeSpace : public iCollisionSpace,public iRegion
{
private:
	struct Block : public cBaseClass {
		iCollisionObject*	mpkFirst;
		u32					muObjectCount;
		union {
			struct {
				f32			mfMinX,mfMinZ,mfMaxX,mfMaxZ;
			};
			vec2_t			mfMPoints[2];	// max,min (y's are z's)
		};
		Block*				mpkChildren;
		Block*				mpkParent;
		s32					miFrustTC;	// generally 0
		
		Block();
		~Block();

		void	blockCreate(const vec3_t &kCenter,f32 fXExt,f32 fZExt,Block* kPar,Block*& kArr,s32 iDepth);
		void	blockAdd(iCollisionObject*);
		void	blockRem(iCollisionObject*);
		void	blockTraverse(iCollisionObject*);
		Block*	blockGet(const aabb_t&);
		Block*	blockGetAbsolute(const aabb_t&);
		void	collide(iCollisionObject*);
		void	collideChildren(iCollisionObject*);
		//void	collide(iCollisionObject*,iCollisionObject*);
		void	cull(iCamera*);
		void	preRenderSubtree();

	};
public:

	//@ sutably the extensions can be infinity
	cQTreeSpace(iRegion* pkParent,const vec3_t &kCenter,f32 fXExt,f32 fZExt,s32 iDepth);
	virtual ~cQTreeSpace();
	//@ Scene Node related:
	virtual iCollisionObject* GetCollisionObject() { return this; }
	virtual void onUpdate(iCamera*);
	virtual void addChildNode(iEntity*);
	virtual void remChildNode(iEntity*);

	//@ Object management
	virtual void addObject(iCollisionObject*);
	virtual void remObject(iCollisionObject*);
	virtual void moveObject(iCollisionObject*);
	virtual void addPortal(iPortal*);
	

	//@ Collision routines
	virtual void onCollide();
	virtual void onExternalCollide(iCollisionObject*);

private:

	void _cleanObjects();

private:

	u32					muBlockCount;
	u32					muGeomCount;
	Block*				mpkRoot;
	list_t<iEntity*>	mkDecorators;
	array_t				mkDirtyList;
	//@ TODO: This is a remove list
	//@	maintained if object is moving out of the
	//@ the space, this is not supposed to happen.
	//@ however for a quad space ,or any complex 
	//@ space. A complex space can have a parent but
	//@ a logical one. And as per rule only portals
	//@ can be used to travel between spaces
//	array_t				mkRemoveList;
};

}

#endif //NEXTAR_QTREECOLLISION_SPACE_H
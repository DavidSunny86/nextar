//@ Abhishek Dey
//@ Oct 20 2007

#ifndef X_IMESH_CORE_H
#define X_IMESH_CORE_H

// ##---	v 1.0
#include <iobject.h>
#include <cstr.h>

namespace nextar
{

class iMeshCore;
class iMesh;

// ##
//@  Currently the mesh core is a top level interface that
//@  will support the following:
//@		<> LOD - all mesh implementation should implement this at a very basic level.
//@		<> Mesh Hierarchy - such that if a parent is called to create a mesh object the sub level 
//@			hierarchy will be created.

class iMeshCore : public iObject
{
protected:
	iMeshCore*	mpkNextMc;
	iMeshCore*	mpkPrevMc;
	iMeshCore*	mpkChildren;
	u32			muFlags;
	cStr		mkName;
public:

	// flags
	enum Flags
	{
		NonAnimatedMCore	=	1 << 0,
	};

	struct iterator
	{
		iMeshCore*	mpkCurrent;

		iterator() : mpkCurrent(0) {}
		iterator(const iterator& it) : mpkCurrent(it.mpkCurrent) {}
		iterator(iMeshCore* p) : mpkCurrent(p) {}
		_n_inline iterator& operator ++ () { mpkCurrent = mpkCurrent->mpkNextMc; return (*this); }
		_n_inline iterator& operator -- () { mpkCurrent = mpkCurrent->mpkPrevMc; return (*this); }
		_n_inline bool operator == ( const iterator& it ) const { return it.mpkCurrent == mpkCurrent; }
		_n_inline bool operator != ( const iterator& it ) const { return it.mpkCurrent != mpkCurrent; }
		_n_inline bool operator !() const { return !mpkCurrent; }
		_n_inline iMeshCore* operator *() { return mpkCurrent; }
		_n_inline bool ok() { return mpkCurrent!=0; }
	};

	//## 
	//@ returns a name describing the mesh core
	_n_inline const tchar*	getName() { return mkName; }
	//##
	//@ return the flags pertaining to this core.
	//@ flags include information like: 
	//@		[] static mesh or animated
	//##
	_n_inline u32				getFlags() { return muFlags; }
	//##
	//@ get child mesh list
	//##
	_n_inline	iterator		getChildren() { return iterator(mpkChildren); }
	//##
	//@ instanciate a mesh object. mesh objects
	//@ can be rendered.
	//##
	virtual iMesh*			instance() = 0;
	//##
	//@ transform the vertices of the mesh and its 
	//@ children by this matrix.
	//##
	virtual void			transform(const f32*) = 0;
	//##
	//@ get the basic material
	//##
	virtual iMaterial*		getType() = 0;
};

}

#endif // X_IMESH_CORE_H
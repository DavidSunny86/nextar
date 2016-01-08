//@ Abhishek Dey
//@ Oct 20 2007

#ifndef X_IMESH_H
#define X_IMESH_H

// ##---	v 1.0
#include "iobject.h"
#include "cstr.h"

namespace nextar
{

class iMeshCore;
class iMesh;

//@ basic support provided by mesh
//@  vertices and indices (copied or referenced)
//@  Material
//@  LOD
//@  mesh hierarchy

class iMesh : public iObject
{
protected:
	iMesh*		mpkNextM;
	u32			muFlags;
	cStr		mkName;
public:

	// flags
	enum Flags
	{
		NonAnimated =   1 << 0,
		StaticMesh	=	1 << 1,
	};

	struct iterator
	{
		iMesh*	mpkCurrent;

		iterator() : mpkCurrent(0) {}
		iterator(const iterator& it) : mpkCurrent(it.mpkCurrent) {}
		iterator(iMesh* p) : mpkCurrent(p) {}
		_n_inline iterator& operator ++ () { mpkCurrent = mpkCurrent->mpkNextM; return (*this); }
	//	_n_inline iterator& operator -- () { mpkCurrent = mpkCurrent->mpkPrevM; return (*this); }
		_n_inline bool operator == ( const iterator& it ) const { return it.mpkCurrent == mpkCurrent; }
		_n_inline bool operator != ( const iterator& it ) const { return it.mpkCurrent != mpkCurrent; }
		_n_inline bool operator !() const { return !mpkCurrent; }
		_n_inline iMesh* operator *() { return mpkCurrent; }
		_n_inline bool ok() { return mpkCurrent!=0; }
	};

	//## 
	//@ returns a name describing the mesh core
	//##
	_n_inline iterator		getThis() { return iterator(this); }
	//## 
	//@ returns a name describing the mesh core
	//##
	_n_inline const tchar*	getName() { return mkName; }
	//##
	//@ return the flags pertaining to this core.
	//@ flags include information like: 
	//@		[] static mesh or animated
	//##
	_n_inline u32				getFlags() { return muFlags; }
	//##
	//@ sets the current lod, value between 0 and 1
	//##
	virtual void			setLod(f32 value) = 0;
	//##
	//@ get the current material
	//##
	virtual iMaterial*		getType() = 0;
	//##
	//@ render
	//##
	virtual void			drawMesh() = 0;
};

}
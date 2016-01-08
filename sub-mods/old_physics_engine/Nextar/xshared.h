//@ Abhishek Dey
//@ Nov 24, 07


/* top level class with selfptrs: members of these */
/* classes can be accessed by a static XSELFPTR pointer */
/* you cannot change the value of this member by any means */
#ifdef X_DEBUG
#	define SharedSingleInterface( _class_ ) protected: static  _class_* spSelfPtr; public: _n_inline static _class_* getPtr() { return spSelfPtr; } _n_inline static void setPtr(_class_* ptr) { spSelfPtr = ptr; } private:
#	define X_SharedPtr( _class_ )		_class_::getPtr()
#else 
#	define SharedSingleInterface( _class_ ) public: static  _class_* spSelfPtr;
#	define X_SharedPtr( _class_ )		_class_::spSelfPtr
#endif

#define SharedSingleInterfaceSetup( _class_ )	X_Api _class_* _class_::spSelfPtr = 0;
#define SharedSingleInterfaceInit( _class_ )	X_Assert(!spSelfPtr); spSelfPtr = this
#define SharedSingleInterfaceExit( _class_ )	X_Assert(spSelfPtr); spSelfPtr = 0

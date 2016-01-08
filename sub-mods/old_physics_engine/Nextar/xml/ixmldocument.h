
/* created : feb 17 07 */
/* by : Abhishek Dey */

#ifndef IXML_NODE_H
#define IXML_NODE_H

#include <xtypes.h>
#include <cstr.h>
#include <iobject.h>
#include <io/ifilereader.h>
#include <io/ifilewriter.h>


namespace nextar
{

/* supports only UTF-8/UTF-16 (according to tchar format) format */
/* very specific and light weight implementation of XML */

/* Much of the code has been borrowed from TinyXML project */
/* For more info visit : http://www.sourceforge.net/projects/tinyxml */

class iXMLDocument;

class X_Api iXMLNode
{
public:

	enum ENODETYPE
	{
		XML_DECLARATION,
		XML_ELEMENT,
		XML_COMMENT,
		XML_TEXT
	};

	struct iXMLIterator
	{
		iXMLNode*	mpData;

		_n_inline iXMLIterator& operator ++ () { if( mpData ) mpData = mpData->mpkNext; return *this; }
		_n_inline iXMLIterator& operator -- () { if( mpData ) mpData = mpData->mpkPrev; return *this; }
		_n_inline iXMLNode*	  operator *()   { return mpData; }
		_n_inline bool		  operator !()   { return mpData == NULL; }
		_n_inline iXMLIterator  next() const { 
			if( mpData ) return iXMLIterator(mpData->mpkNext); return iXMLIterator(); }
		_n_inline iXMLIterator  prev() const { 
			if( mpData ) return iXMLIterator(mpData->mpkPrev); return iXMLIterator(); }
		_n_inline bool		  isValid() { return mpData != NULL; }

		iXMLIterator( iXMLNode* nd ) : mpData(nd) { }
		iXMLIterator() : mpData(0) {}
	};

protected:
	
	iXMLNode( iXMLDocument* );
	virtual ~iXMLNode();	/* kills all child */

	cStr		mkValue;
	iXMLNode	*mpkNext, *mpkPrev;
	iXMLNode	*mpkFirstChild, *mpkLastChild;
	iXMLNode	*mpkParent;
	ENODETYPE	 meType;
	iXMLDocument *mpkDocument;

public:
	
	iXMLNode*		parseNodeType( const tchar* );
	void			addChildLast( iXMLNode* );
	void			addChildFirst( iXMLNode* );
	void			removeChild( iXMLNode* );
	iXMLIterator	removeChild( iXMLIterator );
	iXMLNode*		findChild(const tchar*);

	_n_inline iXMLIterator	nextIt() { return iXMLIterator(mpkNext); }
	_n_inline iXMLIterator	prevIt() { return iXMLIterator(mpkPrev); }
	_n_inline iXMLIterator	thisIt() { return iXMLIterator(this); }
	_n_inline iXMLIterator	firstChildIt() { return iXMLIterator(mpkFirstChild); }
	_n_inline iXMLIterator	nextChildIt( iXMLIterator it ) { return ++it; }
	_n_inline iXMLIterator	prevChildIt( iXMLIterator it ) { return --it; }
	_n_inline iXMLIterator	lastChildIt() { return iXMLIterator(mpkLastChild); }
	_n_inline void			setValue( const cStr& c ) { mkValue = c; }
	_n_inline const cStr&		getValue() const { return mkValue; }
	_n_inline iXMLDocument*	getDocument() { return mpkDocument; }
	_n_inline iXMLNode*		getParent() { return mpkParent; }
	_n_inline void			setType( ENODETYPE eT ) { meType = eT; }
	_n_inline ENODETYPE		getType() { return meType; }

	virtual void		 print( iFileWriter*, s32 depth ) = 0;
	virtual const tchar* parse( const tchar* ) = 0;
};

class X_Api iXMLAttribute
{
private:
	cStr			mkName;
	mutable cStr	mkValue;	
	iXMLAttribute*	mpkNext;
	iXMLAttribute*	mpkPrev;
private:

	void	_getAsNFloat(f32*,s32 n); // new format
	void	_setAsNFloat(const f32*,s32 n); // new format
public:

	_n_inline void			setValue( const cStr& c ) { mkValue = c; }
	_n_inline const cStr&		getValue() const { return mkValue; }
	_n_inline void			setName( const cStr& c ) { mkName = c; }
	_n_inline const cStr&		getName() const { return mkName; }
	_n_inline void			getAs2Float(f32* v) { _getAsNFloat(v,2); }
	_n_inline void			getAs3Float(f32* v) { _getAsNFloat(v,3); }
	_n_inline void			getAs4Float(f32* v) { _getAsNFloat(v,4); }
	_n_inline void			getAs12Float(f32* v) { _getAsNFloat(v,12); }
	_n_inline void			getAs16Float(f32* v) { _getAsNFloat(v,16); }
	_n_inline void			setAs2Float(const f32* v) { _setAsNFloat(v,2); }
	_n_inline void			setAs3Float(const f32* v) { _setAsNFloat(v,3); }
	_n_inline void			setAs4Float(const f32* v) { _setAsNFloat(v,4); }
	_n_inline void			setAs12Float(const f32* v) { _setAsNFloat(v,12); }
	_n_inline void			setAs16Float(const f32* v) { _setAsNFloat(v,16); }

	/* get converted values */
	s32		getAsSignedInt() const;
	u32		getAsUnsignedInt() const;
	u32		getAsHexValue() const;
	f32		getAsFloat() const;
	bool	getAsBoolean() const;
	
	void	setAsSignedInt( s32 );
	void	setAsUnsignedInt( u32 );
	void	setAsHexValue( u32 );
	void	setAsFloat( f32 );
	void	setAsBoolean(bool);

	void		 print( iFileWriter*, s32 depth );
	/* start: first letter of name */
	/* end:   char past quote */
	const tchar* parse( const tchar* );

	iXMLAttribute( const tchar* name );
	iXMLAttribute();
	~iXMLAttribute();	/* will keep the next ptr intact */

	friend class iXMLElement;
};

class X_Api iXMLElement : public iXMLNode
{
private:
	iXMLAttribute		*mpkAttributes;
	s32					miCount;
public:
	
	iXMLElement( iXMLDocument* );
	virtual ~iXMLElement();

	_n_inline s32 getAttributeCount() { return miCount; }

	iXMLAttribute*		 find( const tchar* name );

	iXMLAttribute*		 addAttr( const tchar* name );
	void				 remAttr( const tchar* name );
	
	virtual void		 print( iFileWriter*, s32 depth );
	/*	starts: next char past '<'	*/
	/*	ends:   next char past '>'	*/
	virtual const tchar* parse( const tchar* );

private:

	const tchar* readValue( const tchar* );

	void	addAttr( iXMLAttribute* );
	void	remAttr( iXMLAttribute* );

};

class X_Api iXMLComment : public iXMLNode
{
public:

	iXMLComment( iXMLDocument* );
	virtual ~iXMLComment();

	virtual void		 print( iFileWriter*, s32 depth );
	/*	starts: at the ! of the !-- */
	/*	ends:	next char past '>' */
	virtual const tchar* parse( const tchar* );
};

class X_Api iXMLText : public iXMLNode
{
public:

	iXMLText( iXMLDocument* );
	virtual ~iXMLText();

	bool	isBlank() const;

	virtual void		 print( iFileWriter*, s32 depth );
	/* starts: first char of the text */
	/* end:		next char past '>' */
	virtual const tchar* parse( const tchar* );
};

class X_Api iXMLDeclaration : public iXMLNode
{
private:
	cStr	mkVersion;
	cStr	mkEncoding;
	cStr	mkStandAlone;
public:

	iXMLDeclaration( iXMLDocument* );
	virtual ~iXMLDeclaration();

	_n_inline void			setVersion( const cStr& c ) { mkVersion = c; }
	_n_inline const cStr&		getVersion() const { return mkVersion; }
	_n_inline void			setStandalone( const cStr& c ) { mkStandAlone = c; }
	_n_inline const cStr&		getStandalone() const { return mkStandAlone; }
	_n_inline const cStr&		getEncoding() const { return mkEncoding; }

	virtual void		 print( iFileWriter*, s32 depth );
	/*	starts: next char past '<' */
	/*	ends: next char past '>' */
	virtual const tchar* parse( const tchar* );
};

class X_Api iXMLUnknown : public iXMLNode
{
private:
public:

	iXMLUnknown( iXMLDocument* );
	virtual ~iXMLUnknown();

	virtual void		 print( iFileWriter*, s32 depth );
	/*	starts: next char past '<' */
	/*	ends: next char past '>' */
	virtual const tchar* parse( const tchar* );
};

class iXMLDocument : public iObject, public iXMLNode
{
public:

	enum ENCODING
	{
		UTF_8	=	8,
		UTF_16	=	16
	};


protected:

	static X_Api bool	sbAllowIndent;

	cStr			mkName;
	bool			mbIsOpen;
	bool			mbIsValid;

public:

	iXMLDocument( const tchar* filename );
	virtual ~iXMLDocument();

	_n_inline static void setIndent( bool in ) { sbAllowIndent = in; }
	_n_inline static bool getIndent() { return sbAllowIndent; }

	_n_inline bool isOpen() const { return mbIsOpen; }
	_n_inline bool isValid() const { return mbIsValid; }
	_n_inline void setValid( bool b ) { mbIsValid = b; }
	_n_inline void setName( const cStr& kName ) { mkName = kName; }
	_n_inline const cStr& getName() const { return mkName; }

	static X_Api iXMLDocument* create( const tchar* filename );
	/* IMPORTANT: do not delete the ptrs returned by parse(), getRoot(), etc */
	virtual XERRORCODE  load() = 0;
	virtual XERRORCODE	load( iFileReader *pRead ) = 0; 
	virtual void		save() = 0;
	virtual void		save( iFileWriter *pWrite ) = 0;
	virtual iXMLNode*	createNode( iXMLNode::ENODETYPE ) = 0;
	virtual bool		isValidNextarDocument() { return true; }
	virtual bool		makeValidNextarDocument() { return true; }
};

}

#endif /* IXML_NODE_H */
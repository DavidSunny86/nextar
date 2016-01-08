
//@ Abhishek Dey 
//@ Feb 17 07
//@ Sep 28 07

#include <xmemory.h>
#include <stdio.h>
#include <malloc.h>
#include <xml/ixmldocument.h>
#include <io/ifilesystem.h>

namespace nextar
{

X_Api bool iXMLDocument::sbAllowIndent = true;

_n_inline static const tchar* getChar( const tchar* p, tchar* value );
static const tchar* readText( const tchar* kText, cStr &out, bool bTrim, const tchar* sEndTag, bool );
static void formatString( const cStr& in, cStr& out );
static const tchar* getEntity( const tchar* p, tchar* value );
static const tchar* readName( const tchar* sData, cStr &kName );
static bool stringEqual( const tchar* p, const tchar* tag, bool ignoreCase );

/*cXMLDocument ************************************************************/
class cXMLDocument : public iXMLDocument
{
	FILE*				mpkStream;
	bool				mbParsed;

public:

	enum
	{
		NUM_ENTITY = 5,
		MAX_ENTITY_LENGTH = 6
	};

	struct cBasicEntity
	{
		tchar*	str;
		s32		len;
		tchar	c;		
	};

	static cBasicEntity	skEntities[NUM_ENTITY];

public:

	cXMLDocument( const tchar* file ) : iXMLDocument(file)
	{
		RefObjectInit(cXMLDocument);
		mbIsOpen = false;
		mbIsValid = true;
		mpkStream = 0;
	}

	~cXMLDocument()
	{
		if( mbIsOpen )
			fclose( mpkStream );
	}

	virtual XERRORCODE load()
	{
		if( mbIsOpen ) return X_SUCCESS;
		if( _tfopen_s( &mpkStream, mkName.c_str(), X_Text("rb") ) == 0 )
			mbIsOpen = true;
		else
			return X_ERR_CANNOTOPEN;

		fseek(mpkStream,0,SEEK_END);
		s32 iSize = ftell(mpkStream);
		fseek(mpkStream,0,SEEK_SET);

		if( iSize == 0 )
			return X_ERR_CORRUPTED;

		cStr kFileData;
		{
			tchar* buffer = (tchar*)_alloca((iSize+4));
			kFileData.reserve( iSize+2 );
			if( fread( buffer, iSize, 1, mpkStream ) != 1 ) return X_ERR_CORRUPTED;
			buffer[iSize] = 0;
			kFileData = buffer;
		}

		if( !parse(kFileData.c_str()) )
			return X_ERR_CORRUPTED;

		return X_SUCCESS;
	}

	virtual XERRORCODE load( iFileReader *pRead )
	{
		s32 iSize = pRead->getCurrentSize();
		
		if( iSize == 0 )
			return X_ERR_CORRUPTED;

		cStr kFileData;
		kFileData.reserve( iSize+2 );

		const s32 iBufferSize = 2048;
		tchar  sSmallBuffer[iBufferSize+1];
		s32 iCurrentlyRead = 0;

		while( iCurrentlyRead < iSize )
		{
			s32 iRead = iBufferSize;
			if( iSize-iCurrentlyRead < iBufferSize )
				iRead = iSize-iCurrentlyRead;
			if( !pRead->readData( sSmallBuffer, (u32)iRead ) )
				return X_ERR_CORRUPTED;
			sSmallBuffer[iRead] = 0;
			kFileData += sSmallBuffer;
		}

		if( !parse(kFileData.c_str()) )
			return X_ERR_CORRUPTED;

		return X_SUCCESS;
	}

	virtual const tchar* parse( const tchar* sData )
	{
		if( !sData || !*sData )
			return 0;

		sData = skipWhiteSpace(sData);
		
		if( !sData || !*sData )
			return 0;

		while( sData && *sData )
		{
			iXMLNode* pN = iXMLNode::parseNodeType(sData);
			if( pN )
			{
				sData = pN->parse(sData);
				addChildLast(pN);
			}
			else
				return 0;
			sData = skipWhiteSpace(sData);
		}

		return sData;
	}

	virtual void save()
	{
		iFileWriter *pWrite = X_SharedPtr(iFileSystem)->createFile( mkName );
		if( pWrite ) 
		{
			print( pWrite, 0 );
//		tchar t = _TEOF;
//		pWrite->writeData( &t, sizeof(t) );
			pWrite->release();
		}
	}

	virtual void save( iFileWriter *pWrite )
	{
		print( pWrite, 0 );
//		tchar t = _TEOF;
//		pWrite->writeData( &t, sizeof(t) );
	}

	virtual void print( iFileWriter* pWrite, s32 depth )
	{
		if( !iXMLDocument::getIndent() )
			depth = 0;

		iXMLNode::iXMLIterator it = firstChildIt();
		// check if we have a declration node, if not add it
		tchar txt[] = X_Text("\n");
		while(it.isValid())
		{
			(*it)->print( pWrite, depth );
			pWrite->writeData(txt,sizeof(txt));
			++it;
		}
	}

	iXMLNode* createNode( iXMLNode::ENODETYPE t )
	{
		switch(t)
		{
		case iXMLNode::XML_COMMENT:
			return new iXMLComment(this);
		case iXMLNode::XML_DECLARATION:
			return new iXMLDeclaration(this);
		case iXMLNode::XML_ELEMENT:
			return new iXMLElement(this);
		case iXMLNode::XML_TEXT:
			return new iXMLText(this);
		default:
			return 0;
		}
	}
};

cXMLDocument::cBasicEntity cXMLDocument::skEntities[cXMLDocument::NUM_ENTITY] = 
{
	{ X_Text("&amp;"),  5, '&' },
	{ X_Text("&lt;"),   4, '<' },
	{ X_Text("&gt;"),   4, '>' },
	{ X_Text("&quot;"), 6, '\"' },
	{ X_Text("&apos;"), 6, '\'' }
};
/**************************************************************************/

/*iXMLDocument ************************************************************/
iXMLDocument::iXMLDocument( const tchar* file ) : iXMLNode(0), mkName(file)
{
	mpkDocument = this;
}

iXMLDocument::~iXMLDocument()
{
}

X_Api iXMLDocument* iXMLDocument::create(const tchar *filename)
{
	return new cXMLDocument( filename );
}

/**************************************************************************/

/*iXMLNode ************************************************************/
iXMLNode::iXMLNode( iXMLDocument* doc ) : mpkNext(0), mpkPrev(0), mpkFirstChild(0), mpkLastChild(0), mpkParent(0), mpkDocument(doc) 
{
}

iXMLNode::~iXMLNode()
{
	iXMLIterator it = firstChildIt();
	iXMLNode::iXMLIterator nextit;
	while(it.isValid())
	{
		nextit = it.next();
		delete (*it);
		it = nextit;
	}
}


iXMLNode* iXMLNode::parseNodeType( const tchar* p )
{
	iXMLNode* returnNode = 0;

	p = skipWhiteSpace( p );

	if( !p || !*p || *p != '<' )
		return 0;

	iXMLDocument* doc = getDocument();

	// What is this thing? 
	// - Elements start with a letter or underscore, but xml is reserved.
	// - Comments: <!--
	// - Decleration: <?xml
	// - Everthing else is unknown to tinyxml.
	//
	const tchar* xmlHeader = { X_Text("<?xml") };
	const tchar* commentHeader = { X_Text("<!--") };

	if ( stringEqual( p, xmlHeader, true ) )
		returnNode = new iXMLDeclaration(doc);
	else if ( isalpha( *(p+1) )  || *(p+1) == '_' )
		returnNode = new iXMLElement(doc);
	else if ( stringEqual( p, commentHeader, false ) )
		returnNode = new iXMLComment(doc);
	else
		returnNode = new iXMLUnknown(doc);

	if ( returnNode )
		// Set the parent, so it can report errors
		returnNode->mpkParent = this;
	return returnNode;
}

void iXMLNode::addChildFirst( iXMLNode *pkNode )
{
	X_BreakOutIf(!pkNode);
	pkNode->mpkParent = this;
	pkNode->mpkNext = mpkFirstChild;
	if( mpkFirstChild ) 
		mpkFirstChild->mpkPrev = pkNode;
	mpkFirstChild = pkNode;
	if( !mpkLastChild ) mpkLastChild = mpkFirstChild;
}

void iXMLNode::addChildLast( iXMLNode *pkNode )
{

	X_BreakOutIf(!pkNode);
	pkNode->mpkParent = this;
	pkNode->mpkPrev = mpkLastChild;
	if( mpkLastChild ) 
		mpkLastChild->mpkNext = pkNode;
	mpkLastChild = pkNode;
	if( !mpkFirstChild ) mpkFirstChild = mpkLastChild;
}

void iXMLNode::removeChild( iXMLNode *pkNode )
{
	X_BreakOutIf(!pkNode);
	if( pkNode->getParent() == this )
	{
		pkNode->mpkParent = 0;
		iXMLIterator it = firstChildIt();
		while(it.isValid())
		{
			if( it.mpData == pkNode )
			{
				removeChild(it);
				return;
			}
			++it;
		}
	}
}

iXMLNode::iXMLIterator iXMLNode::removeChild( iXMLNode::iXMLIterator it )
{
	X_BreakOutIf( !it.isValid() );
	iXMLNode* pkP = (*it.prev());
	iXMLNode* pkN = (*it.next());
	if(pkP) pkP->mpkNext = pkN;
	if(pkN) pkN->mpkPrev = pkP;
	if(*it == mpkLastChild) mpkLastChild = pkP;
	if(*it == mpkFirstChild) mpkFirstChild = pkN;
	return iXMLNode::iXMLIterator(pkN);
}

iXMLNode* iXMLNode::findChild(const tchar* sName)
{
	cStr kName(sName);
	iXMLIterator it = firstChildIt();
	while(it.isValid())
	{
		if( (*it)->getValue().noCaseCompare(kName) )
			return (*it);
		++it;
	}
	return 0;
}

/**************************************************************************/

/*iXMLAttribute ***********************************************************/

iXMLAttribute::iXMLAttribute( const tchar* name ) : mkName(name)
{
}

iXMLAttribute::iXMLAttribute()
{
}

iXMLAttribute::~iXMLAttribute()
{
}

s32 iXMLAttribute::getAsSignedInt() const
{
	return _ttol(mkValue.c_str());
}

u32 iXMLAttribute::getAsUnsignedInt() const
{
	return _tcstoul(mkValue.c_str(),0,10);
}

u32 iXMLAttribute::getAsHexValue() const
{
	u32 ret = 0;
	const tchar* str = mkValue.c_str();
	s32 maxpow = (mkValue.strlen()-1)*4;
	while(*str)
	{
		u32 num = 0;
		if(*str >= 'a' && *str <= 'f')
			num = (*str-'a')+10;
		else if(*str >= 'A' && *str <= 'F')
			num = (*str-'A')+10;
		else if(*str >= '0' && *str <= '9')
			num = (*str-'0');
		ret += num << maxpow;
		maxpow-=4;
		str++;
	}
	return ret;
}

f32 iXMLAttribute::getAsFloat() const
{
	return (f32)_tstof(mkValue.c_str());
}

bool iXMLAttribute::getAsBoolean() const
{
	const cStr true_s("true");
	mkValue.trim();
	return mkValue.noCaseCompare(true_s);
}

void iXMLAttribute::_getAsNFloat(f32* v,s32 n)
{
	X_Assert(n>=1);
	tchar* str = mkValue.str();
	tchar* epr = 0;
	while(*str!='{'){ str++; if(!*str) return; }
	++str;
	while(*str==' ' || *str=='\t' || *str=='\n') { str++; if(!*str) return; }
	v[0] = (f32)_tcstod(str,&epr);
	str = epr;
	for(s32 i=1; i<n;i++)
	{
		while(*str!=','){ str++; if(!*str) return; }
		while(*str==' ' || *str=='\t' || *str=='\n') { str++; if(!*str) return; }
		v[i] = (f32)_tcstod(str,&epr);
		str = epr;
	}
}

void iXMLAttribute::setAsSignedInt( s32 v )
{
	mkValue = v;
}

void iXMLAttribute::setAsUnsignedInt( u32 v )
{
	mkValue = v;
}

void iXMLAttribute::setAsHexValue( u32 v )
{
	mkValue.reserve(8);
	while( v )
	{
		tchar c = (tchar)(v & 0xf0000000);
		if(c>9) c = (c-10) + 'a';
		else c += '0';
		mkValue.append(c);
		v <<= 4;
	}
}

void iXMLAttribute::setAsFloat( f32 v )
{
	mkValue = v;
}

void iXMLAttribute::setAsBoolean(bool b)
{
	if(b) mkValue = "true";
	else mkValue = "false";
}

void iXMLAttribute::_setAsNFloat(const f32* v,s32 n)
{
	cStr kAppStr;
	mkValue = X_Text("{");
	s32 i=0;
	for(;i<n-1;i++)
	{
		kAppStr.format(" %f,",20,v[i]);
		mkValue.append(kAppStr);
	}
	
	kAppStr.format(" %f }",20,v[i]);
	mkValue.append(kAppStr);
}

void iXMLAttribute::print( iFileWriter* pWriter, s32 )
{
	cStr kN,kV,kW;
	
	formatString(mkName,kN);
	formatString(mkValue,kV);
	s32 iReserved = kN.strlen()+kV.strlen()+6;
	
	if( mkValue.findchar('\"') == -1 )
		kW.format(X_Text("%s=\"%s\""), iReserved, kN.c_str(), kV.c_str() );
	else
		kW.format(X_Text("%s='%s'"), iReserved, kN.c_str(), kV.c_str() );
	pWriter->writeData( kW.str(), kW.strlen()*sizeof(tchar) );
}

const tchar* iXMLAttribute::parse( const tchar * sData )
{
	sData = skipWhiteSpace( sData );
	if ( !sData || !*sData ) return 0;

	// Read the name, the '=' and the value.
	sData = readName( sData, mkName );
	if ( !sData || !*sData )
		return 0;
	sData = skipWhiteSpace( sData );

	if ( !sData || !*sData || *sData != '=' )
		return 0;

	++sData;	// skip '='
	sData = skipWhiteSpace( sData );

	if ( !sData || !*sData )
		return 0;
	
	const tchar* end;

	if ( *sData == '\'' )
	{
		++sData;
		end = X_Text("\'");
		sData = readText( sData, mkValue, false, end, false );
	}
	else if ( *sData == '"' )
	{
		++sData;
		end = X_Text("\"");
		sData = readText( sData, mkValue, false, end, false );
	}
	else
	{
		// All attribute values should be in single or double quotes.
		// But this is such a common error that the parser will try
		// its best, even without them.
		mkValue = X_Text("");
		while (    sData && *sData										// existence
				&& !isspace( *sData ) && *sData != '\n' && *sData != '\r'	// whitespace
				&& *sData != '/' && *sData != '>' )						// tag end
		{
			mkValue += *sData;
			++sData;
		}
	}
	return sData;
}
/**************************************************************************/

/*iXMLElement *************************************************************/
iXMLElement::iXMLElement( iXMLDocument* doc ) : iXMLNode(doc), mpkAttributes(0), miCount(0)
{
}

iXMLElement::~iXMLElement()
{
	while(mpkAttributes)
	{
		iXMLAttribute* a = mpkAttributes->mpkNext;
		delete mpkAttributes;
		mpkAttributes = a;
	}
}

iXMLAttribute* iXMLElement::addAttr(const tchar *name)
{
	iXMLAttribute* a = new iXMLAttribute(name);
	addAttr(a);
	return a;
}

void iXMLElement::remAttr(const tchar *name)
{
	iXMLAttribute* a = find(name);
	if(a) remAttr(a);
}

iXMLAttribute* iXMLElement::find(const tchar *name)
{
	iXMLAttribute* a = mpkAttributes;
	while(a)
	{
		if(a->mkName.noCaseCompare(name))
			return a;
		a = a->mpkNext;
	}
	return 0;
}

void iXMLElement::addAttr( iXMLAttribute* att )
{
	X_BreakOutIf(!att);
	att->mpkNext = mpkAttributes;
	if(mpkAttributes) 
		mpkAttributes->mpkPrev = att;
	mpkAttributes = att;
	miCount++;
}

void iXMLElement::remAttr( iXMLAttribute *a )
{
	X_BreakOutIf(!a);
	if(a->mpkPrev) a->mpkPrev->mpkNext = a->mpkNext;
	if(a->mpkNext) a->mpkNext->mpkPrev = a->mpkPrev;
	miCount--;
	delete a;
}

const tchar* iXMLElement::readValue( const tchar* p )
{
	// Read in text and elements in any order.
	p = skipWhiteSpace(p);
	while ( p && *p )
	{
		if ( *p != '<' )
		{
			// Take what we have, make a text element.
			iXMLText* pT = new iXMLText(getDocument());

			if ( !pT )
				return 0;
		
			p = pT->parse( p );

			if ( !pT->isBlank() )
				addChildLast( pT );
			else
				delete pT;
		}
		else 
		{
			// We hit a '<'
			// Have we hit a new element or an end tag?
			if ( stringEqual( p, "</", false ) )
				return p;
			else
			{
				iXMLNode* node = parseNodeType( p );
				if ( node )
				{
					p = node->parse( p );
					addChildLast( node );
				}				
				else
					return 0;
			}
		}
		p = skipWhiteSpace( p );
	}
	return p;
}

const tchar* iXMLElement::parse( const tchar *p )
{	
	p = skipWhiteSpace( p );

	if ( !p || !*p || *p != '<' )
		return 0;

	p = skipWhiteSpace( p+1 );

	// Read the name.
	p = readName( p, mkValue );
	
	if ( !p || !*p )
		return 0;

    cStr endTag(X_Text("</"));
	endTag += mkValue;
	endTag += X_Text(">");

	// Check for and read attributes. Also look for an empty
	// tag or an end tag.
	while ( p && *p )
	{
		p = skipWhiteSpace( p );
		if ( !p || !*p )
			return 0;
		if ( *p == '/' )
		{
			++p;
			// Empty tag.
			if ( *p  != '>' )
				return 0;
			return (p+1);
		}
		else if ( *p == '>' )
		{
			// Done with attributes (if there were any.)
			// Read the value -- which can include other
			// elements -- read the end tag, and return.
			++p;
			p = readValue( p );		// Note this is an Element method, and will set the error if one happens.
			if ( !p || !*p )
				return 0;

			// We should find the end tag now
			if ( stringEqual( p, endTag.c_str(), false ) )
			{
				p += endTag.strlen();
				return p;
			}
			else
				return 0;
		}
		else
		{
			// Try to read an element:
			iXMLAttribute* attrib = new iXMLAttribute;

			p = attrib->parse( p );
			
			if ( !p || !*p )
			{
				delete attrib;
				return 0;
			}
			addAttr( attrib );
		}
	}
	return p;
}

void iXMLElement::print( iFileWriter *pWrite,  s32 depth )
{
	cStr kWrite = X_Text("");
	s32 i;

	for ( i=0; i<depth; i++ )
		kWrite += X_Text("    ");

	pWrite->writeData( kWrite.str(), kWrite.strlen()*sizeof(tchar) );

	kWrite.format(X_Text("<%s"), mkValue.strlen()+3, mkValue.c_str() );

	pWrite->writeData( kWrite.str(), kWrite.strlen()*sizeof(tchar) );

	iXMLAttribute* attrib;
	for ( attrib = mpkAttributes; attrib; attrib = attrib->mpkNext )
	{
		pWrite->writeData( X_Text(" "), 1*sizeof(tchar) );	
		attrib->print( pWrite, depth );
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	if ( !mpkFirstChild )
	{
		pWrite->writeData( X_Text(" />"), sizeof(X_Text(" />"))-sizeof(tchar) );
	}
	else if ( mpkFirstChild == mpkLastChild && mpkFirstChild->getType() == iXMLNode::XML_TEXT )
	{
		pWrite->writeData( X_Text(">"), 1*sizeof(tchar) );
		mpkFirstChild->print( pWrite, depth + 1 );
		goto endtag;
	}
	else
	{
		pWrite->writeData( X_Text(">"), 1*sizeof(tchar) );
		for ( iXMLNode::iXMLIterator it = firstChildIt(); it.isValid(); ++it )
		{
			if ( (*it)->getType() != iXMLNode::XML_TEXT )
				pWrite->writeData( X_Text("\n"), 1*sizeof(tchar) );
			(*it)->print( pWrite, depth+1 );
		}

		pWrite->writeData( X_Text("\n"), 1*sizeof(tchar) );

		kWrite = X_Text("");
		for( i=0; i<depth; ++i )
			kWrite += X_Text("    ");
endtag:
		kWrite.format( X_Text("</%s>"), mkValue.strlen()+5,mkValue.c_str());
		pWrite->writeData( kWrite.str(), (kWrite.strlen()-1)*sizeof(tchar) );
	}
}

/**************************************************************************/

/*iXMLComment *************************************************************/
iXMLComment::iXMLComment( iXMLDocument *doc ) : iXMLNode(doc)
{
}

iXMLComment::~iXMLComment()
{
}

const tchar* iXMLComment::parse( const tchar *p )
{
	mkValue = "";

	p = skipWhiteSpace( p );
	const char* startTag = "<!--";
	const char* endTag   = "-->";

	if ( !stringEqual( p, startTag, false ) )
		return 0;

	p += 4;
	p = readText( p, mkValue, false, endTag, false );
	return p;
}

void iXMLComment::print( iFileWriter *pWrite,  s32 depth )
{
	cStr kWrite;

	for ( s32 i=0; i<depth; i++ )
		kWrite += "    ";

	pWrite->writeData( kWrite.str(), kWrite.strlen()*sizeof(tchar) );
	kWrite.format( "<!--%s-->", mkValue.strlen() + 9, mkValue.c_str() );
	pWrite->writeData( kWrite.str(), kWrite.strlen()*sizeof(tchar) );
}
/**************************************************************************/

/*iXMLText ****************************************************************/
iXMLText::iXMLText( iXMLDocument *doc ) : iXMLNode(doc)
{
}

iXMLText::~iXMLText()
{
}

const tchar* iXMLText::parse( const tchar *p )
{
	mkValue = "";

	bool ignoreWhite = true;
	const char* end = "<";
	p = readText( p, mkValue, ignoreWhite, end, false );
	if ( p )
		return p-1;	// don't truncate the '<'
	return 0;
}

void iXMLText::print( iFileWriter *pWrite,  s32 )
{
	cStr kBuffer;
	formatString( mkValue, kBuffer );
	pWrite->writeData( kBuffer.str(), kBuffer.strlen()*sizeof(tchar) );
}

bool iXMLText::isBlank() const
{
	return mkValue.isNull();
}

/**************************************************************************/

/*iXMLDeclaration *********************************************************/
iXMLDeclaration::iXMLDeclaration( iXMLDocument *doc ) : iXMLNode(doc)
{
}

iXMLDeclaration::~iXMLDeclaration()
{
}

const tchar* iXMLDeclaration::parse( const tchar *p )
{
	p = skipWhiteSpace( p );
	// Find the beginning, find the end, and look for
	// the stuff in-between.
	if ( !p || !*p || !stringEqual( p, X_Text("<?xml"), true ) )
		return 0;

	p += 5;

	mkVersion = X_Text("");
	mkEncoding = X_Text("");
	mkStandAlone = X_Text("");

	while ( p && *p )
	{
		if ( *p == '>' )
		{
			++p;
			return p;
		}

		p = skipWhiteSpace( p );
		if ( stringEqual( p, "version", true ) )
		{
			iXMLAttribute attrib;
			p = attrib.parse( p );		
			mkVersion = attrib.getValue();
		}
		else if ( stringEqual( p, "encoding", true ) )
		{
			iXMLAttribute attrib;
			p = attrib.parse( p );		
			mkEncoding = attrib.getValue();
		}
		else if ( stringEqual( p, "standalone", true ) )
		{
			iXMLAttribute attrib;
			p = attrib.parse( p );		
			mkStandAlone = attrib.getValue();
		}
		else
		{
			// Read over whatever it is.
			while( p && *p && *p != '>' && !isspace( *p ) )
				++p;
		}
	}
	return 0;
}

void iXMLDeclaration::print( iFileWriter *pWrite, s32 )
{
	cStr kBuffer;

	pWrite->writeData( X_Text("<?xml "), sizeof(X_Text("<?xml "))-sizeof(tchar));

	if ( !mkVersion.strlen() )
	{
		kBuffer.format(X_Text("version=\"%s\" "), 15 + mkVersion.strlen(), mkVersion.c_str() );
		pWrite->writeData( kBuffer.str(), kBuffer.strlen()*sizeof(tchar) );
	}
	if ( !mkEncoding.strlen() )
	{
		kBuffer.format(X_Text("encoding=\"%s\" "), 16 + mkEncoding.strlen(), mkEncoding.c_str() );
		pWrite->writeData( kBuffer.str(), kBuffer.strlen()*sizeof(tchar) );
	}
	if ( !mkStandAlone.strlen() )
	{
		kBuffer.format(X_Text("standalone=\"%s\" "), 17 + mkStandAlone.strlen(), mkStandAlone.c_str() );
		pWrite->writeData( kBuffer.str(), kBuffer.strlen()*sizeof(tchar) );
	}
	pWrite->writeData( X_Text("?>"), sizeof(X_Text("?>"))-sizeof(tchar));
}
/**************************************************************************/

/*iXMLUnknown *************************************************************/

iXMLUnknown::iXMLUnknown( iXMLDocument *doc ) : iXMLNode(doc)
{
}

iXMLUnknown::~iXMLUnknown()
{
}

const tchar* iXMLUnknown::parse( const tchar *p )
{
	p = skipWhiteSpace( p );
	if ( !p || !*p || *p != '<' )
		return 0;
	++p;
	mkValue = X_Text("");

	while ( p && *p && *p != '>' )
	{
		mkValue.append(*p);
		++p;
	}

	if ( !p )
		return 0;

	if ( *p == '>' )
		return p+1;
	return p;
}

void iXMLUnknown::print( iFileWriter *pWrite, s32 depth )
{
	cStr kWrite;

	for ( s32 i=0; i<depth; i++ )
		kWrite += "    ";

	pWrite->writeData( kWrite.str(), kWrite.strlen()*sizeof(tchar) );
	pWrite->writeData( mkValue.str(), mkValue.strlen()*sizeof(tchar) );
}

/**************************************************************************/

/*globals *****************************************************************/
static void formatString( const cStr& in, cStr& out )
{
	s32 i=0;

	while( i<(s32)in.strlen() )
	{
		tchar c = in[i];

		if ( c == '&' && i < ( in.strlen() - 2 ) && in[i+1] == '#' && in[i+2] == 'x' )
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			while ( i<in.strlen() )
			{
				out.append( in.c_str() + i, 1 );
				++i;
				if ( in[i] == ';' )
					break;
			}
		}
		else 
		{
			bool found = false;
			for( s32 j = 0; j < cXMLDocument::NUM_ENTITY; j++ )
			{
				if( cXMLDocument::skEntities[j].c == c )
				{
					out.append( cXMLDocument::skEntities[j].str, cXMLDocument::skEntities[j].len );
					++i;
					found = true;
					break;
				}
			}

			if( !found )
			{
				if ( c < 32 || c > 126 )
				{
					// Easy pass at non-alpha/numeric/symbol
					// 127 is the delete key. Below 32 is symbolic.
					tchar buf[ 32 ];
					_stprintf_s( buf, 32, X_Text("&#x%02X;"), (unsigned) ( c & 0xff ) );
					out.append( buf, (s32)strlen( buf ) );
					++i;
				}
				else
				{
					tchar realc = (tchar) c;
					out.append( &realc, 1 );
					++i;
				}
			}
		}
	}
}

static const tchar* readName( const tchar* sData, cStr &kName )
{
	kName = X_Text("");
	X_Assert( sData );

	// Names start with letters or underscores.
	// After that, they can be letters, underscores, numbers,
	// hyphens, or colons. (Colons are valid ony for namespaces,
	// but tinyxml can't tell namespaces from names.)
	if ( sData && *sData && ( isalpha( (tchar) *sData ) || *sData == '_' ) )
	{
		while(	*sData	&&	( isalnum( (tchar) *sData ) 
			|| *sData == '_' || *sData == '-' || *sData == ':' ) )
		{
			kName.append(*sData);
			++sData;
		}
		return sData;
	}
	return 0;
}

static bool stringEqual( const tchar* p, const tchar* tag, bool ignoreCase )
{
	X_Assert(p);
	if ( !*p )
		return false;

	if ( tolower( *p ) == tolower( *tag ) )
	{
		const tchar* q = p;

		if (ignoreCase)
		{
			while ( *q && *tag && *q == *tag )
			{
				++q;
				++tag;
			}

			if ( *tag == 0 )		// Have we found the end of the tag, and everything equal?
			{
				return true;
			}
		}
		else
		{
			while ( *q && *tag && tolower( *q ) == tolower( *tag ) )
			{
				++q;
				++tag;
			}

			if ( *tag == 0 )
			{
				return true;
			}
		}
	}
	return false;
}

static const tchar* getEntity( const tchar* p, tchar* value )
{
	// Presume an entity, and pull it out.
    cStr ent;
	s32 i;

	// Ignore the &#x entities.
	if ( _tcsnccmp( X_Text("&#x"), p, 3 ) == 0  && *(p+3) && *(p+4) )
	{
		*value = 0;
		
		if ( isalpha( *(p+3) ) ) 
			*value += ( tolower( *(p+3) ) - 'a' + 10 ) * 16;
		else				     
			*value += ( *(p+3) - '0' ) * 16;

		if ( isalpha( *(p+4) ) ) 
			*value += ( tolower( *(p+4) ) - 'a' + 10 );
		else	
			*value += ( *(p+4) - '0' );

		return p+6;
	}

	// Now try to match it.
	for( i=0; i<cXMLDocument::NUM_ENTITY; ++i )
	{
		if ( _tcsnccmp( cXMLDocument::skEntities[i].str, p, cXMLDocument::skEntities[i].len ) == 0 )
		{
			*value = cXMLDocument::skEntities[i].c;
			return ( p + cXMLDocument::skEntities[i].len );
		}
	}

	// So it wasn't an entity, its unrecognized, or something like that.
	*value = *p;	// Don't put back the last one, since we return it!
	return p+1;
}

_n_inline static const tchar* getChar( const tchar* p, tchar* value )
{
	X_BreakOutIf(!p);
	if ( *p == '&' )
		return getEntity( p, value );
	else
		*value = *p;
	return p+1;
}

static const tchar* readText( const tchar* sData, cStr &kText, bool bTrim, const tchar* sEndTag, bool bCase )
{
	kText = X_Text("");
	if ( !bTrim								 /* certain tags always keep whitespace */
		|| iXMLDocument::getIndent() ) 	 /* if true, whitespace is always kept */
	{
		/* Keep all the white space. */
		while (	sData && *sData	&& !stringEqual( sData, sEndTag, bCase )  )
		{
			tchar c;
			sData = getChar( sData, &c );
			kText.append(c);
		}
	}
	else
	{
		bool whitespace = false;

		// Remove leading white space:
		sData = skipWhiteSpace( sData );
		while (  sData && *sData && !stringEqual( sData, sEndTag, bCase ) )
		{
			if ( *sData == '\r' || *sData == '\n' )
			{
				whitespace = true;
				++sData;
			}
			else if ( isspace( *sData ) )
			{
				whitespace = true;
				++sData;
			}
			else
			{
				// If we've found whitespace, add it before the
				// new character. Any whitespace just becomes a space.
				if ( whitespace )
				{
					kText.append(' ');
					whitespace = false;
				}
				tchar c;
				sData = getChar( sData, &c );
				kText.append(c);
			}
		}
	}
	return sData + _tcslen( sEndTag );
}
/**************************************************************************/

}
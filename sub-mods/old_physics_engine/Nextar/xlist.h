/*VARCHECK*/
/*FILE MARK
 DOC ORIGINAL NAME:	list_t.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	10/29/2006		AT:		1:18:14 PM
END
******************************************************************************************************
*/
#ifndef __XLIST_H__
#define __XLIST_H__

#include <xtypes_c.h>
namespace nextar
{

template < typename T >
class list_t : public cBaseClass
{

public:

	typedef s32 size_type;

	struct Node
	{
		T	  mtObj;
		Node* mpNext;
		Node* mpPrev;

		_n_inline Node() : mpNext(0), mpPrev(0) {}
		_n_inline Node( const T& Obj ) : mpNext(0), mpPrev(0) { mtObj = Obj; }
		_n_inline Node( const T& Obj, Node *Next, Node *Prev ) : mpNext(Next), mpPrev(Prev) { mtObj = Obj; }
	};

	class iterator
	{
	public:

		_n_inline iterator() : mpCurrent(0) {}
		_n_inline iterator( Node* pNode ) : mpCurrent(pNode) { }
		_n_inline iterator(const iterator& it) : mpCurrent(it.mpCurrent) {}

		_n_inline iterator& operator ++ () { mpCurrent = mpCurrent->mpNext; return (*this); }
		_n_inline iterator& operator -- () { mpCurrent = mpCurrent->mpPrev; return (*this); }
		_n_inline iterator  next() { return iterator(mpCurrent->mpNext); }
		_n_inline iterator  prev() { return iterator(mpCurrent->mpPrev); }
		_n_inline iterator  operator ++ (int) { iterator it = *this; mpCurrent = mpCurrent->mpNext; return it; }
		_n_inline iterator  operator -- (int) { iterator it = *this; mpCurrent = mpCurrent->mpPrev; return it; }

		_n_inline iterator& operator += (int amt) {
			
			if( amt > 0 )
				while( amt-- && mpCurrent ) mpCurrent = mpCurrent->mpNext;
			else
				while( amt++ && mpCurrent ) mpCurrent = mpCurrent->mpPrev;

			return *this;
		}

		_n_inline iterator& operator -= (int amt) {
			
			if( amt < 0 )
				while( amt++ && mpCurrent ) mpCurrent = mpCurrent->mpNext;
			else
				while( amt-- && mpCurrent ) mpCurrent = mpCurrent->mpPrev;

			return *this;
		}

		_n_inline iterator operator + (int amt) const {
		
			iterator it( *this );

			if( amt > 0 )
				while( amt-- && it.mpCurrent ) it.mpCurrent = it.mpCurrent->mpNext;
			else
				while( amt++ && it.mpCurrent ) it.mpCurrent = it.mpCurrent->mpPrev;

			return it;
		}
		
		_n_inline iterator operator - (int amt) const {
		
			iterator it( *this );

			if( amt < 0 )
				while( amt++ && it.mpCurrent ) it.mpCurrent = it.mpCurrent->mpNext;
			else
				while( amt-- && it.mpCurrent ) it.mpCurrent = it.mpCurrent->mpPrev;

			return it;
		}

		_n_inline bool operator == ( const iterator& it ) const { return it.mpCurrent == mpCurrent; }
		_n_inline bool operator != ( const iterator& it ) const { return it.mpCurrent != mpCurrent; }
		_n_inline bool operator !() const { return !mpCurrent; }
		_n_inline T& operator *() { return mpCurrent->mtObj; }
		_n_inline bool ok() { return mpCurrent!=0; }
		
		Node* mpCurrent;
	};

private:

	Node*		mpHead;
	Node*		mpTail;
	size_type	mnSize;

public:

	_n_inline list_t() : mpHead(0), mpTail(0), mnSize(0) {}
	_n_inline list_t( const list_t &_sList ) : mpHead(0), mpTail(0), mnSize(0) { assign(_sList.begin(), _sList.end()); }

	_n_inline iterator  begin() const { return iterator(mpHead); }
	_n_inline iterator  back() const { return iterator(mpTail); }
	_n_inline iterator  end() const  { return iterator(); }
	_n_inline size_type size()  { return mnSize; }
	_n_inline bool	  empty() { return mpHead == 0; }
	
	// clear
	void  clear() 
	{ 
		Node* n = mpHead;
		while( n  )
		{
			Node* nx = n->mpNext;
			delete n;
			n = nx;
		}
		mpHead = 0;
		mpTail = 0;
		mnSize = 0;
	}
	
	/************************************
	./ assign	
	./ Decscription: <similar to STL> Assigns to list_t deleting all its existing contents. Assigns [begin,end).
	./ Return type:	void 
	./ param:  iterator begin [in]:		
	./ param: iterator end	[in]:		
	************************************/
	void assign( const iterator begin_iterator, const iterator end_iterator )
	{
		Node* Tmp(mpHead);
		bool  TheEndOfList = false;
		
		iterator tmp = begin_iterator;
		/* Assign */
		if( !mpHead && begin_iterator != end_iterator ) Tmp = mpHead = mpTail = new Node();

		while( tmp != end_iterator )
		{
			/* Create node only when needed */
			if( TheEndOfList ) 
			{ 
				Tmp = new Node( *tmp, 0, mpTail );
				mpTail->mpNext = Tmp;
				mpTail = Tmp;
				mnSize++; 
				Tmp = 0;
			}
			else 
			{ 
				Tmp->mtObj = *tmp;
				if( !Tmp->mpNext ) TheEndOfList = true;
				else Tmp = Tmp->mpNext;
			}

			++tmp;
		}

		/* Clear rest of the list_t */
		erase( iterator(Tmp), iterator() );
	}

	/************************************
	./ assign	
	./ Decscription: <STL style> Assign from start nCnt elements with Obj and erase rest of the list_t.
	./ Return type:	void 
	./ param:  size_type nCnt	[in]:	Total Count	
	./ param: const T &Obj		[in]:	Object
	************************************/
	void assign( size_type nCnt, const T &Obj )
	{
		Node* Tmp(mpHead);
		bool  TheEndOfList = false;

		/* Assign */
		if( !mpHead && nCnt > 0 ) Tmp = mpHead = mpTail = new Node();

		while( nCnt > 0 )
		{
			/* Create node only when needed */
			if( TheEndOfList ) 
			{ 
				Tmp = new Node( Obj, 0, mpTail );
				mpTail->mpNext = Tmp;
				mpTail = Tmp;
				++mnSize;
				Tmp = 0;
			}
			else 
			{ 
				Tmp->mtObj = Obj;
				if( !Tmp->mpNext ) TheEndOfList = true;
				else Tmp = Tmp->mpNext;
			}
		}

		/* Clear rest of the list_t */
		erase( Iteartor(Tmp), iterator() );
	}

	/************************************
	./ erase	
	./ Decscription: <STL type erase -> [begin,end). Make sure the iterators belog to this
	./ list_t. Using other list_t's iterator will work but will always result in an internal error 
	./ whose detection will not be possible resulting in memory leaks.
	./ Return type:	void , returns nothing unlike STL
	./ param:  iterator begin	[in]:		
	./ param: iterator end		[in]:		
	************************************/
	void erase( iterator begin_iterator, iterator end_iterator )
	{
		iterator tmp = begin_iterator;
		if( begin_iterator.mpCurrent == mpHead ) 
		{
			mpHead = end_iterator.mpCurrent;
			if( !end_iterator.mpCurrent ) mpTail = 0;
		}
		else if( !end_iterator.mpCurrent )
			mpTail = begin_iterator.mpCurrent->mpPrev; 

		Node* n = begin_iterator.mpCurrent;
		Node* e = end_iterator.mpCurrent;
		while( n != e )
		{
			Node* nx = n?n->mpNext : 0;
			if(n)
			{
				if(n->mpNext) n->mpNext->mpPrev = n->mpPrev;
				if(n->mpPrev) n->mpPrev->mpNext = n->mpNext;
				delete n;
				--mnSize;
			}
			n = nx;
		}
	}
	/************************************
	./ erase	
	./ Decscription: <STL type erase -> it. Make sure the iterators belog to this
	./ list_t. Using other list_t's iterator will work but will always result in an internal error 
	./ whose detection will not be possible resulting in memory leaks.
	./ Return type:	void, returns nothing unlike STL
	./ param:  iterator it	[in]:		
	************************************/
	void erase( iterator it )
	{
		X_BreakOutIf( !it );
		if( mpHead == it.mpCurrent ) mpHead = it.mpCurrent->mpNext;
		if( mpTail == it.mpCurrent ) mpTail = it.mpCurrent->mpPrev;
		if( it.mpCurrent->mpNext ) it.mpCurrent->mpNext->mpPrev = it.mpCurrent->mpPrev;
		if( it.mpCurrent->mpPrev ) it.mpCurrent->mpPrev->mpNext = it.mpCurrent->mpNext;
		delete it.mpCurrent;
		--mnSize;
	}
	
	/************************************
	./ pop_back	
	./ Decscription: Removes the first element	
	./ Return type:	void 
	************************************/
	void pop_back() 
	{
		if( mpTail )
		{
			Node* n = mpTail;
			mpTail = mpTail->mpPrev;
			delete n;
			--mnSize;
		}
	}

	/************************************
	./ pop_front	
	./ Decscription: Removes the first element
	./ Return type:	void 
	************************************/
	void pop_front()
	{
		if( mpHead )
		{
			Node* n = mpHead;
			mpHead = mpHead->mpNext;
			delete n;
			--mnSize;
		}
	}

	/************************************
	./ push_back	
	./ Decscription: Attaches to the tail
	./ Return type:	void 
	./ param: const T& obj	[in]:		
	************************************/
	void push_back( const T& obj )
	{
		Node *n = new Node( obj, 0, mpTail );
		++mnSize;
		if( mpTail ) mpTail->mpNext = n;
		else mpHead = n;
		mpTail = n;
	}
	
	/************************************
	./ push_front	
	./ Decscription:  Attaches to the head
	./ Return type:	void 
	./ param: const T& obj	[in]:		
	************************************/
	void push_front( const T& obj )
	{
		Node *n = new Node( obj, mpHead, 0 );
		++mnSize;
		if( mpHead ) mpHead->mpPrev = n;
		else mpTail = n;
		mpHead = n;
	}

	/************************************
	./ insert	
	./ Decscription: STL insert, insert after the current iterator it
	./ Return type:	void, does not return
	./ param:  iterator it	[in]:	0 here would mean push_back
	./ param: const T &obj	[in]:		
	************************************/
	void insert( iterator it, const T &obj )
	{
		if( !it ) 
			push_back(obj);
		else if( it.mpCurrent == mpHead )
			push_front(obj);
		else
		{
			Node *n = new Node( obj, it.mpCurrent, it.mpCurrent->mpPrev );
			++mnSize;
			it.mpCurrent->mpPrev->mpNext = n;
			it.mpCurrent->mpPrev = n;
		}
	}

	/************************************
	./ remove	
	./ Decscription: STL type remove
	./ Return type:	void 
	./ param: const T& Obj	[]:		
	************************************/
	void remove( const T& Obj )
	{
		Node* pTemp = mpHead;

		while( pTemp )
		{
			if( pTemp->mtObj == Obj )
			{
				if(pTemp->mpPrev) pTemp->mpPrev->mpNext = pTemp->mpNext;
				else mpHead = pTemp->mpNext;
				if(pTemp->mpNext) pTemp->mpNext->mpPrev = pTemp->mpPrev;
				else mpTail = pTemp->mpPrev;
				delete pTemp;
				return;
			}

			pTemp = pTemp->mpNext;
		}
	}
};

}

#endif //__XLIST_H__
//@ Abhishek Dey
//@ Nov 09 2007

template <typename T>
class SpecialList {

	T*	mpkBegin;
	T*	mpkEndActive;
	T*	mpkEnd;
	s32 miCount;
	
public:
	SpecialList() : mpkBegin(0),mpkEndActive(0),mpkEnd(0),miCount(0) {}
	~SpecialList() {}

	_n_inline s32		size() { return miCount; }
	_n_inline T*		begin() { return mpkBegin; }
	_n_inline T*		end() { return mpkEnd; }
	_n_inline T*		endactive() { return mpkEndActive; }
	
	_n_inline void add(T* el) 
	{ 
		if(mpkBegin)
		{
			mpkBegin->mpkPrev = el;
			el->mpkNext = mpkBegin;
		}
		else
			el->mpkNext = 0;
		el->mpkPrev = 0;
		mpkBegin = el;
		if(!mpkEnd) mpkEnd = el;
		++miCount;
	}
	_n_inline void rem(T* el) 
	{
		if(el == mpkEndActive)
			mpkEndActive = el->mpkNext;
		if(el->mpkPrev)
			el->mpkPrev->mpkNext = el->mpkNext;
		else
			mpkBegin = el->mpkNext;
		if(el->mpkNext)
			el->mpkNext->mpkPrev = el->mpkPrev;
		else
			mpkEnd = el->mpkPrev;
		--miCount;
	}
	_n_inline T* deactivate(T* el) 
	{
		// attach after tail
		T* ret = el->mpkNext;
		if(ret == mpkEndActive)
		{
			mpkEndActive = el;
			return mpkEndActive;
		}
		if(mpkEndActive)
		{
			if(el->mpkPrev)
				el->mpkPrev->mpkNext = el->mpkNext;
			else
				mpkBegin = el->mpkNext;

			X_Assert(el->mpkNext)
			el->mpkNext->mpkPrev = el->mpkPrev;
		
			mpkEnd->mpkNext = el;
			el->mpkNext = 0;
			el->mpkPrev = mpkEnd;
			mpkEnd = el;
		}
		else
		{
			if(mpkEnd != el)
			{
				if(el->mpkPrev)
					el->mpkPrev->mpkNext = el->mpkNext;
				else
					mpkBegin = el->mpkNext;

				el->mpkNext->mpkPrev = el->mpkPrev;
				el->mpkPrev = mpkEnd;
				el->mpkNext = 0;
				if(mpkEnd) 
					mpkEnd->mpkNext = el;
				mpkEnd = el;
			}
			mpkEndActive = el;
		}
		return ret;
	}
	_n_inline T* activate(T* el)
	{
		X_Assert(mpkBegin);
		X_Assert(mpkEndActive);

		T* ret = el->mpkNext;
		if(el == mpkEndActive)
			mpkEndActive = el->mpkNext;
		if(el->mpkPrev)
			el->mpkPrev->mpkNext = el->mpkNext;
		else
			mpkBegin = el->mpkNext;
		if(el->mpkNext)
			el->mpkNext->mpkPrev = el->mpkPrev;
		else
			mpkEnd = el->mpkPrev;
		if(mpkBegin) 
		{
			mpkBegin->mpkPrev = el;
			el->mpkNext = mpkBegin;
			el->mpkPrev = 0;
			mpkBegin = el;
		}
		else
		{
			X_Assert(miCount==1);
			mpkBegin = el;
			el->mpkPrev = 0;
			el->mpkNext = 0;
		}
		return ret;
	}
};

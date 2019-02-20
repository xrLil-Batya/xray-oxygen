#pragma once
#include "xrDebug_macros.h"

template <class T, std::size_t dim>
class svector 
{
public:
	typedef	size_t				size_type;
	typedef T					value_type;
	typedef value_type*			iterator;
	typedef const value_type*	const_iterator;
	typedef value_type&			reference;
	typedef const value_type&	const_reference;

private:
	value_type	vArray[dim];
	u32			count;

public:
	svector() : count(0) 
	{}
	svector(iterator p, int c) 
	{ assign(p,c); }

	IC iterator	begin()						{ return vArray;						}
	IC iterator	end	 ()						{ return vArray+count;					}
	IC const_iterator	cbegin()	const	{ return vArray;						}
	IC const_iterator	cend	 ()	const	{ return vArray+count;					}
	IC u32		size()		const			{ return count;							}
	IC void		clear()						{ count=0;								}
	IC void		resize(int c)				{ VERIFY(c<=dim); count=c;				}
	IC void		reserve(int c)				{ }

	IC void		push_back(value_type e)		{ VERIFY(count<dim); vArray[count++]=e;	}
	IC void		pop_back()					{ VERIFY(count); count--;				}

	IC reference		operator[] (u32 id)			{ VERIFY_FORMAT(id < count, "id=[%u],count=[%u]", id, count); return vArray[id]; }
	IC const_reference	operator[] (u32 id)	const	{ VERIFY_FORMAT(id < count, "id=[%u],count=[%u]", id, count); return vArray[id]; }

	IC reference		front()				{ return vArray[0];						}
	IC reference		back()				{ return vArray[count-1];				}
	IC reference		last()				{ VERIFY(count<dim); return vArray[count];}
	IC const_reference	front() const		{ return vArray[0];						}
	IC const_reference	back()  const		{ return vArray[count-1];				}
	IC const_reference	last()  const		{ VERIFY(count<dim); return vArray[count];}
	IC void		inc	()						{ count++; }
	IC bool		empty()		const			{ return 0==count;	}

	IC void		erase(u32 id)				{
		VERIFY(id<count);
		count--;
		for (u32 i=id; i<count; i++)
			vArray[i] = vArray[i+1];
	}
	IC void		erase(iterator it)				{ erase(u32(it-begin()));	}

	IC void		insert(u32 id, reference V)
	{
		VERIFY(id<count);
		for (int i=count; i>int(id); i--)	vArray[i] = vArray[i-1];
		count++;
		vArray[id] = V;
	}
	IC void		assign(iterator p, int c) { VERIFY(c>0 && c<dim); std::memcpy(vArray,p,c*sizeof(value_type)); count=c; }
	IC BOOL		equal (const svector<value_type,dim>& base) const
	{
		if (size()!=base.size())	return FALSE;
		for (u32 cmp=0; cmp<size(); cmp++)	if ((*this)[cmp]!=base[cmp])	return FALSE;
		return TRUE;
	}
};

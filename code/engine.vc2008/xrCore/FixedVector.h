#ifndef FixedVectorH
#define FixedVectorH
#pragma once

template <class T, std::size_t dim>
class svector
{
public:
	typedef	size_t size_type;
	typedef T value_type;
	typedef value_type* iterator;
	typedef const value_type*	const_iterator;
	typedef value_type& reference;
	typedef const value_type&	const_reference;

private:
	value_type	array[dim];
	u32			count;

public:
	svector() : count(0)
	{}
	svector(iterator p, int c)
	{
		assign(p, c);
	}

	inline iterator	begin() { return array; }
	inline iterator	end() { return array + count; }
	inline const_iterator	begin()	const { return array; }
	inline const_iterator	end()	const { return array + count; }
	inline u32 size() const { return count; }
	inline void clear() { count = 0; }
	inline void resize(int c) { VERIFY(c <= dim); count = c; }
	inline void reserve(int c) { }

	inline void push_back(value_type e) { VERIFY(count < dim); array[count++] = e; }
	inline void pop_back() { VERIFY(count); count--; }

	inline reference		operator[] (u32 id) { VERIFY2(id < count, make_string("id=[%d] count=[%d]", id, count)); return array[id]; }
	inline const_reference	operator[] (u32 id)	const { VERIFY2(id < count, make_string("id=[%d] count=[%d]", id, count)); return array[id]; }

	inline reference front() { return array[0]; }
	inline reference back() { return array[count - 1]; }
	inline reference last() { VERIFY(count < dim); return array[count]; }
	inline const_reference	front() const { return array[0]; }
	inline const_reference	back()  const { return array[count - 1]; }
	inline const_reference	last()  const { VERIFY(count < dim); return array[count]; }
	inline void inc() { count++; }
	inline bool empty()		const { return 0 == count; }

	inline void erase(u32 id)
	{
		VERIFY(id < count);
		count--;
		for (u32 i = id; i < count; ++i)
			array[i] = array[i + 1];
	}
	inline void		erase(iterator it) { erase(u32(it - begin())); }

	inline void		insert(u32 id, reference V)
	{
		VERIFY(id < count);
		for (int i = count; i > int(id); i--)	array[i] = array[i - 1];
		count++;
		array[id] = V;
	}
	inline void assign(iterator p, int c) { VERIFY(c > 0 && c < dim); std::memcpy(array, p, c * sizeof(value_type)); count = c; }
	inline BOOL equal(const svector<value_type, dim>& base) const
	{
		if (size() != base.size())
			return FALSE;

		for (u32 cmp = 0; cmp < size(); cmp++)
			if ((*this)[cmp] != base[cmp])
				return FALSE;

		return TRUE;
	}
};

#endif
#ifndef CYCLE_CONST_STORAGE_H
#define CYCLE_CONST_STORAGE_H

template<class T, int size>
class CCycleConstStorage
{
	T array[size];
	int first;
	inline int position(int i) const { return (first + i) % size; }
public:
	inline CCycleConstStorage() { first = 0; }

	inline void fill_in(const T& val) { std::fill(array, array + size, val); }

	inline void push_back(T& val)
	{
		array[first] = val;
		first = position(1);
	}

	inline T& operator [] (int i) { return array[position(i)]; }

	inline const T& operator [] (int i) const { return array[position(i)]; }
};
#endif
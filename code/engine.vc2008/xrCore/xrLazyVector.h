// Giperion August 2019
// [EUREKA] 3.10.3
// X-Ray Oxygen, Oxygen Team

//////////////////////////////////////////////////////////////
// Desc		: Special type container, that do not free memory too often. For single-threaded code only!
// Author	: Giperion
//////////////////////////////////////////////////////////////
// Oxygen Engine 2016-2019
//////////////////////////////////////////////////////////////
#pragma once
#include <type_traits>

// Require for some rare cases, when storage used very frequently with "clean and refill" mechanic
template<typename TypeElement>
struct xrLazyVector
{
	xrLazyVector()
	{}

	xrLazyVector(u32 preAllocateSize)
	{
		_container = (TypeElement*)Memory.mem_alloc(sizeof(TypeElement) * preAllocateSize);
		_capacity = preAllocateSize;
	}


	struct iterator
	{
		xrLazyVector<TypeElement>* source;
		size_t index;

		// shit for std begin
		using iterator_category = iterator;
		using value_type = TypeElement;
		using difference_type = size_t;
		using pointer = TypeElement *;
		using reference = TypeElement &;
		// shit for std end

		iterator(xrLazyVector<TypeElement>* InVector, size_t InIndex)
			: source (InVector), index(InIndex)
		{}

		iterator()
			: source(nullptr), index(0)
		{}

		iterator(const iterator& other)
		{
			source = other.source;
			index = other.index;
		}

		iterator& operator=(const iterator& other)
		{
			source = other.source;
			index = other.index;
			return *this;
		}

		TypeElement* operator->() const
		{
			return &(*source)[index];
		}

		TypeElement& operator*() const
		{
			return (*source)[index];
		}

		iterator& operator++()
		{
			index++;
			return *this;
		}

		iterator operator++(int)
		{
			index++;
			return *this;
		}

		iterator& operator--(int)
		{
			index--;
			return *this;
		}

		iterator& operator--()
		{
			index--;
			return *this;
		}

		bool operator !=(const iterator& other) const
		{
			return source != other.source || index != other.index;
		}

		bool operator ==(const iterator& other) const
		{
			return source == other.source && index == other.index;
		}

		bool operator !=(const TypeElement& other) const
		{
			return (*source)[index] != other;
		}

		bool operator ==(const TypeElement& other) const
		{
			return (*source)[index] == other;
		}

		iterator operator+(const iterator& other) const
		{
			return iterator(source, index + other.index);
		}

		iterator operator+(const difference_type& other) const
		{
			return iterator(source, index + other);
		}

		iterator operator-(const size_t& other) const
		{
			return iterator(source, index - other);
		}

		iterator operator-(const int& other) const
		{
			return iterator (source, index - other);
		}

		difference_type operator-(const iterator& other) const
		{
			return index - other.index;
		}

		bool operator > (const iterator& other) const
		{
			return index > other.index;
		}

		bool operator < (const iterator& other) const
		{
			return index < other.index;
		}
	};

	struct reverse_iterator
	{
		xrLazyVector<TypeElement>* source;
		long long index;

		// shit for std begin
		using iterator_category = iterator;
		using value_type = TypeElement;
		using difference_type = long long;
		using pointer = TypeElement *;
		using reference = TypeElement &;
		// shit for std end

		reverse_iterator(xrLazyVector<TypeElement>* InVector, long long InIndex)
			: source(InVector), index(InIndex)
		{}

		reverse_iterator()
			: source(nullptr), index(0)
		{}

		reverse_iterator(const reverse_iterator& other)
		{
			source = other.source;
			index = other.index;
		}

		reverse_iterator& operator=(const reverse_iterator& other)
		{
			source = other.source;
			index = other.index;
			return *this;
		}

		TypeElement* operator->() const
		{
			return &(*source)[index];
		}

		TypeElement& operator*() const
		{
			return (*source)[index];
		}

		reverse_iterator& operator++()
		{
			index--;
			return *this;
		}

		reverse_iterator operator++(int)
		{
			index--;
			return *this;
		}

		reverse_iterator& operator--(int)
		{
			index++;
			return *this;
		}

		reverse_iterator& operator--()
		{
			index++;
			return *this;
		}

		bool operator !=(const reverse_iterator& other) const
		{
			return source != other.source || index != other.index;
		}

		bool operator ==(const reverse_iterator& other) const
		{
			return source == other.source && index == other.index;
		}

		bool operator !=(const TypeElement& other) const
		{
			return (*source)[index] != other;
		}

		bool operator ==(const TypeElement& other) const
		{
			return (*source)[index] == other;
		}

		reverse_iterator operator+(const reverse_iterator& other) const
		{
			return reverse_iterator(source, index - other.index);
		}

		reverse_iterator operator+(const difference_type& other) const
		{
			return reverse_iterator(source, index - other);
		}

		reverse_iterator operator-(const long long& other) const
		{
			return reverse_iterator(source, index + other);
		}

		reverse_iterator operator-(const int& other) const
		{
			return reverse_iterator(source, index + other);
		}

		difference_type operator-(const reverse_iterator& other) const
		{
			return index + other.index;
		}

		bool operator > (const reverse_iterator& other) const
		{
			return index < other.index;
		}

		bool operator < (const reverse_iterator& other) const
		{
			return index > other.index;
		}
	};

	struct const_iterator
	{
		const xrLazyVector<TypeElement>* source;
		size_t index;

		// shit for std begin
		using iterator_category = iterator;
		using value_type = TypeElement;
		using difference_type = size_t;
		using pointer = TypeElement *;
		using reference = TypeElement &;
		// shit for std end

		const_iterator(const xrLazyVector<TypeElement>* InVector, size_t InIndex)
			: source(InVector), index(InIndex)
		{}

		const_iterator()
			: source(nullptr), index(0)
		{}

		const_iterator(const const_iterator& other)
		{
			source = other.source;
			index = other.index;
		}

		const_iterator& operator=(const const_iterator& other)
		{
			source = other.source;
			index = other.index;
			return *this;
		}

		TypeElement* operator->() const
		{
			return &(*source)[index];
		}

		TypeElement& operator*() const
		{
			return (*source)[index];
		}

		const_iterator& operator++()
		{
			index++;
			return *this;
		}

		const_iterator operator++(int)
		{
			index++;
			return *this;
		}

		const_iterator& operator--(int)
		{
			index--;
			return *this;
		}

		const_iterator& operator--()
		{
			index--;
			return *this;
		}

		bool operator !=(const const_iterator& other) const
		{
			return source != other.source || index != other.index;
		}

		bool operator ==(const const_iterator& other) const
		{
			return source == other.source && index == other.index;
		}

		bool operator !=(const TypeElement& other) const
		{
			return (*source)[index] != other;
		}

		bool operator ==(const TypeElement& other) const
		{
			return (*source)[index] == other;
		}

		const_iterator operator+(const const_iterator& other) const
		{
			return const_iterator(source, index + other.index);
		}

		const_iterator operator+(const difference_type& other) const
		{
			return const_iterator(source, index + other);
		}

		const_iterator operator-(const size_t& other) const
		{
			return const_iterator(source, index - other);
		}

		const_iterator operator-(const int& other) const
		{
			return const_iterator(source, index - other);
		}

		difference_type operator-(const const_iterator& other) const
		{
			return index - other.index;
		}

		bool operator > (const const_iterator& other) const
		{
			return index > other.index;
		}

		bool operator < (const const_iterator& other) const
		{
			return index < other.index;
		}
	};

	void clearNoFree()
	{
		_size = 0;
	}

	void clear()
	{
		if (_container != nullptr)
		{
			_size = 0;
			_capacity = 0;
			Memory.mem_free(_container);
			_container = nullptr;
		}
	}

	void trimSpace()
	{
		if (empty())
		{
			clear();
		}
		else
		{
			_container = (TypeElement*)Memory.mem_realloc(_container, _size * sizeof(TypeElement));
			_capacity = _size;
		}
	}

	TypeElement& operator[] (size_t index) const
	{
		VERIFY_FORMAT(index < _size, "Index out of range. Request %zu, but have %u", index, _size);
		return _container[index];
	}

	size_t size() const
	{
		return _size;
	}

	bool empty() const
	{
		return _size == 0;
	}

	iterator begin()
	{
		if (_container != nullptr)
		{
			return iterator(this, 0);
		}
		return iterator(this, 0);
	}

	iterator end()
	{
		if (_container != nullptr)
		{
			return iterator(this, _size);
		}
		return iterator(this, 0);
	}

	const_iterator cbegin() const
	{
		if (_container != nullptr)
		{
			return const_iterator(this, 0);
		}
		return const_iterator(this, 0);
	}

	const_iterator cend() const
	{
		if (_container != nullptr)
		{
			return const_iterator(this, _size);
		}
		return const_iterator(this, 0);
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator(this, _size - 1);
	}

	reverse_iterator rend()
	{
		return reverse_iterator(this, -1);
	}

	iterator erase(const iterator elem)
	{
		size_t lastElements = (_size - 1) - elem.index;
		if constexpr (!std::is_fundamental<TypeElement>::value && !std::is_pointer<TypeElement>::value)
		{
			elem->~TypeElement();
		}

		if (lastElements > 0)
		{
			memmove(&_container[elem.index], &_container[elem.index + 1], lastElements * sizeof(TypeElement));
		}
		_size--;

		return iterator(this, elem.index);
	}

	iterator erase(const iterator first, const iterator second)
	{
		size_t start = first.index;
		size_t end = second.index;
		if constexpr (!std::is_fundamental<TypeElement>::value && !std::is_pointer<TypeElement>::value)
		{
			for (iterator iter = first; iter != second; iter++)
			{
				iter->~TypeElement();
			}
		}

		size_t lastElements = (_size - 1) - end;

		if (lastElements > 0)
		{
			memmove(&_container[start], &_container[end], lastElements * sizeof(TypeElement));
		}

		_size = _size - (end - start);
		return iterator(this, start);
	}

	void remove(TypeElement elem)
	{
		for (auto iter = begin(); iter != end(); iter++)
		{
			if (*iter == elem)
			{
				erase(iter);
				return;
			}
		}
	}

	void reserve(size_t NewSize)
	{
		VERIFY(NewSize > 0);

		if (_capacity == NewSize) return;

		if (_capacity < NewSize)
		{
			_container = (TypeElement*)Memory.mem_realloc(_container, NewSize * sizeof(TypeElement));
			_capacity = NewSize;
		}
	}

	void resize(size_t NewSize)
	{
		VERIFY(NewSize > 0);
		if (_size == NewSize) return;

		// reserve and construct objects
		if (_size < NewSize)
		{
			reserve(NewSize);

			if constexpr (!std::is_fundamental<TypeElement>::value && !std::is_pointer<TypeElement>::value)
			{
				for (size_t i = _size; i < NewSize; i++)
				{
					new (&_container[i])TypeElement();
				}
			}
		}
		else
		{
			if constexpr (!std::is_fundamental<TypeElement>::value && !std::is_pointer<TypeElement>::value)
			{
				for (size_t i = _size; i < NewSize; --i)
				{
					_container[i].~TypeElement();
				}
			}
		}

		_size = NewSize;
	}

	void push_back(TypeElement& elem)
	{
		if (_capacity < (_size + 1))
		{
			reserve(_size + 10);
		}

		new(&_container[_size])TypeElement(elem);
		_size++;
	}

	void push_back(TypeElement&& elem)
	{
		if (_capacity < (_size + 1))
		{
			reserve(_size + 10);
		}

		new(&_container[_size])TypeElement(elem);
		_size++;
	}

	TypeElement& front() const
	{
		return _container[0];
	}

	TypeElement& back() const
	{
		return _container[_size - 1];
	}
	
	void pop_back()
	{
		resize(_size - 1);
	}

	void insert(const iterator Where, const iterator Begin, const iterator End)
	{
		// get size and ending
		size_t Diff = End - Begin;

		iterator Dest(this, Where.index + Diff);
		reserve(Dest.index);
		_size = Dest.index;
		iterator sourceIt = Begin;
		for (iterator it = Where; it != Dest; it++)
		{
			*it = *sourceIt;
			sourceIt++;
		}
	}

private:

	TypeElement* _container = nullptr;
	size_t _size = 0;
	size_t _capacity = 0;
};

//template<typename InType>
//typename xrLazyVector<InType>::iterator  operator - (typename const xrLazyVector<InType>::iterator& v1, typename const xrLazyVector<InType>::iterator& v2)
//{
//	return xrLazyVector<InType>::iterator(v1.source, v1.index - v2.index);
//}

//template<typename InType>
//typename xrLazyVector<InType>::iterator  operator + (typename const xrLazyVector<InType>::iterator& v1, typename const xrLazyVector<InType>::iterator::difference_type& v2)
//{
//	return xrLazyVector<InType>::iterator(v1.source, v1.index + v2);
//}
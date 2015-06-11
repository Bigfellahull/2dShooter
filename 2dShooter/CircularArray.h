#pragma once

#include <vector>

namespace SGS2D
{
	template <typename T>
	class CircularArray
	{
	public:
		CircularArray(int capacity)
		{
            _list.resize(capacity);
		}

		int GetStart() { return _start; }
		void SetStart(int value) { _start = value % _list.size(); }
		int GetCount() { return _count; }
		void SetCount(int value) { _count = value; }
		int GetCapacity() { return _list.size(); }

		T& operator [](const int i)
		{
			return _list[(_start + i) % _list.size()];
		}

		const T& operator [](const int i) const
		{
			return _list[(_start + i) % _list.size()];
		}
	private:
		std::vector<T> _list;
		int _start;
		int _count;
	};
}
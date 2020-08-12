#pragma once

#include<iostream>
#include<assert.h>
using namespace std;
// 用const代替宏，调试可以看见
const size_t MAX_SIZE = 64 * 1024;
const size_t NFREE_LIST = MAX_SIZE / 8;
inline void*& NextObj(void* obj)
{
	return *((void**)obj);
}
class FreeList
{
public:
	void Push(void* obj)
	{
		// 头插
		NextObj(obj) = _freelist;
		_freelist = obj;
	}

	void PushRange(void* head, void* tail)
	{
		NextObj(tail) = _freelist;
		_freelist = head;
	}

	void* Pop()
	{
		// 头删
		void* obj = _freelist;
		_freelist = NextObj(obj);
		return obj;
	}

	bool Empty()
	{
		return _freelist == nullptr;
	}
private:
	void* _freelist = nullptr;
};

class SizeClass
{
public:
	/*static size_t ListIndex(size_t size)
	{
		if (size % 8 == 0)
		{
			return size / 8 - 1;
		}
		else
		{
			return size / 8;
		}
	}*/
	// 范围在[2, 512]个之间
	static size_t NumMoveSize(size_t size)
	{
		if (size == 0)
			return 0;
		int num = MAX_SIZE / size;
		if (num < 2)
			num = 2;
		if (num > 512)
			num = 512;
			return num;
	}

	/*static size_t RoundUp(size_t size)
	{
		if (size % 8 !== 0)
		{
			return (size / 8 + 1) * 8;
		}
	}*/
	
	static size_t _RoundUp(size_t size, int alignment)
	{
		return (size + alignment - 1) & (~(alignment - 1));
	}
	static inline size_t RoundUp(size_t size)
	{
		assert(size <= MAX_SIZE);

		if (size <= 128)
		{
			return _RoundUp(size, 8);
		}
		else if (size <= 1024)
		{
			return _RoundUp(size, 16);
		}
		else if (size <= 8192)
		{
			return _RoundUp(size, 128);
		}
		else if (size <= 65536)
		{
			return _RoundUp(size, 1024);
		}
		return -1;
	}

	static size_t _ListIndex(size_t size, int align_shift)
	{
		return ((size + (1 << align_shift) - 1) >> align_shift) - 1;
	}
	static size_t ListIndex(size_t size)
	{
		assert(size <= MAX_SIZE);

		// 每个区间有多少个链
		static int group_array[4] = { 16, 56, 56, 112 }; 
		if (size <= 128) 
		{
			return _ListIndex(size, 3);
		}
		else if (size <= 1024) 
		{
			return _ListIndex(size - 128, 4) + group_array[0];
		}
		else if (size <= 8192) 
		{
			return _ListIndex(size - 1024, 7) + group_array[1] + group_array[0];
		}
		else if (size <= 65536) 
		{
			return _ListIndex(size - 8192, 10) + group_array[2] + group_array[1] +
				group_array[0];
		}

		assert(false); 
		return -1;
	}
};

// Span是管理以页为单位的内存对象，作用是方便合并，解决内存碎片
// 针对windows
#ifdef _WIN32
typedef unsigned int PAGE_ID;

#else
typedef unsigned long long PAGE_ID;
#endif // _WIN32

struct Span
{
	PAGE_ID _pageid;   // 页号
	int _pagesize;     // 页的数量

	void* _freelist;   // 对象的自由链表
	int _usecount;     // 内存块对象使用计数

	//size_t objsize;    // 对象大小
	Span* _next;
	Span* _prev;
};

class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	void PushFront(Span* newspan)
	{
		Insert(_head->_next, newspan);
	}

	void PopFront(Span* newspan)
	{
		Erase(_head->_next);
	}

	void PushBack(Span* newspan)
	{
		Insert(_head->_prev, newspan);
	}

	void PopBack(Span* newspan)
	{
		Erase(_head->_prev);
	}

	void Insert(Span* pos, Span* newspan)
	{
		Span* prev = pos->_prev;
		// prev newspan pos
		prev->_next = newspan;
		newspan->_next = pos;
		newspan->_prev = prev;
		pos->_prev = newspan;
	}

	void Erase(Span* pos)
	{
		assert(pos != _head);

		Span* prev = pos->_prev;
		Span* next = pos->_next;

		prev->_next = next;
		next->_prev = prev;
    // 不能delete掉pos，因为这不是new出来的，而是下一层PageCache切出来的，不用了就还给PageCache
	}
private:
	Span* _head;
};
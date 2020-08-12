#pragma once

#include<iostream>
#include<assert.h>
using namespace std;
// ��const����꣬���Կ��Կ���
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
		// ͷ��
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
		// ͷɾ
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
	// ��Χ��[2, 512]��֮��
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

		// ÿ�������ж��ٸ���
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

// Span�ǹ�����ҳΪ��λ���ڴ���������Ƿ���ϲ�������ڴ���Ƭ
// ���windows
#ifdef _WIN32
typedef unsigned int PAGE_ID;

#else
typedef unsigned long long PAGE_ID;
#endif // _WIN32

struct Span
{
	PAGE_ID _pageid;   // ҳ��
	int _pagesize;     // ҳ������

	void* _freelist;   // �������������
	int _usecount;     // �ڴ�����ʹ�ü���

	//size_t objsize;    // �����С
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
    // ����delete��pos����Ϊ�ⲻ��new�����ģ�������һ��PageCache�г����ģ������˾ͻ���PageCache
	}
private:
	Span* _head;
};
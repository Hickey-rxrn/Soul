
#include"ThreadCache.h"


void* ThreadCache::Allocte(size_t size)
{
	size_t index = SizeClass::ListIndex(size); 
	FreeList& freeList = _freeLists[index];
	if (!freeList.Empty())
	{
		return freeList.Pop();
	}
	else
	{
		return FetchFromCentralCache(size);
	}
}
void ThreadCache::Deallocte(void* ptr, size_t size)
{
	size_t index = SizeClass::ListIndex(size); 
	FreeList& freeList = _freeLists[index];
	freeList.Push(ptr);

	//if ()
	{
		//ReleaseToCentralCache();
	}
}
// 独立测试 thread cache
//void* ThreadCache::FetchFromCentralCache(size_t size)
//{
//	size_t num = 20; // 以后调整
//	// 模拟从中心缓存取内存对象的代码，测试thread cache的逻辑
//	size_t size = (index + 1) * 8;
//	char* start = (char*)malloc(size * num);
//	char* cur = start;
//	for (size_t i = 0; i < num - 1; ++i)
//	{
//		char* next = cur + size;
//		NextObj(cur) = next;
//
//		cur = next;
//	}
//
//	NextObj(cur) = nullptr;
//
//	void* head = NextObj(start);
//	void* tail = cur;
//
//	_freeLists[index].PushRange(head, tail);
//	return start;
//}
void* ThreadCache::FetchFromCentralCache(size_t size)
{
	size_t num = SizeClass::NumMoveSize(size);  
	return nullptr;
}
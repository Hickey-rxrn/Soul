
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
// �������� thread cache
//void* ThreadCache::FetchFromCentralCache(size_t size)
//{
//	size_t num = 20; // �Ժ����
//	// ģ������Ļ���ȡ�ڴ����Ĵ��룬����thread cache���߼�
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
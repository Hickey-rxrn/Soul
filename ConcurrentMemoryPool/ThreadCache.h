#pragma once

#include"Common.h"

class ThreadCache
{
public:
	// 申请内存
	void* Allocte(size_t size);
	// 释放内存
	void Deallocte(void* ptr, size_t size);
	// 从中心缓存获取对象
	void* FetchFromCentralCache(size_t index);
private:
	FreeList _freeLists[NFREE_LIST];   //自由链表
};
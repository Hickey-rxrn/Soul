#pragma once

#include "Common.h"

class CentralCache
{
public:
	// 从中心缓存获取一定量的对象给ThreadCache,与FetchCentralCache对接
	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t size);
	// 将一定数量的对象释放到span跨度
	void ReleaseListToSpans(void* start, size_t size);
	// 从PageCache获取一个span
	Span* GetOneSpan(SpanList* list, size_t size);
private:
	SpanList _spanLists[NFREE_LIST];
};
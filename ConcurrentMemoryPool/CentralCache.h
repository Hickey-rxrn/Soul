#pragma once

#include "Common.h"

class CentralCache
{
public:
	// �����Ļ����ȡһ�����Ķ����ThreadCache,��FetchCentralCache�Խ�
	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t size);
	// ��һ�������Ķ����ͷŵ�span���
	void ReleaseListToSpans(void* start, size_t size);
	// ��PageCache��ȡһ��span
	Span* GetOneSpan(SpanList* list, size_t size);
private:
	SpanList _spanLists[NFREE_LIST];
};
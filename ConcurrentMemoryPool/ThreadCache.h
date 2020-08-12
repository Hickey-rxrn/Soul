#pragma once

#include"Common.h"

class ThreadCache
{
public:
	// �����ڴ�
	void* Allocte(size_t size);
	// �ͷ��ڴ�
	void Deallocte(void* ptr, size_t size);
	// �����Ļ����ȡ����
	void* FetchFromCentralCache(size_t index);
private:
	FreeList _freeLists[NFREE_LIST];   //��������
};
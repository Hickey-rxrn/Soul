
#include"ThreadCache.h"

#include<vector>
void UnitThreadCache()
{
	ThreadCache tc;
	vector<void*> v;
	for (size_t i = 0; i < 21; ++i)
	{
		v.push_back(tc.Allocte(7));
	}

	for (size_t i = 0; i < v.size(); ++i)
	{
		printf("[%d]->%p\n", i, v[i]);
	}

	for (auto ptr : v)
	{
		tc.Deallocte(ptr, 7);
	}
}
//
//void UnitTestSizeClass()
//{
//	cout << SizeClass::RoundUp(9, 8) << endl;
//	cout << SizeClass::RoundUp(12, 8) << endl;
//	cout << SizeClass::RoundUp(14, 8) << endl;
//	cout << SizeClass::RoundUp(16, 8) << endl;
//	cout << SizeClass::RoundUp(17, 8) << endl;
//}

void UnitTestRoundUp()
{
	cout << SizeClass::RoundUp(1) << endl;
	cout << SizeClass::RoundUp(127) << endl;
	cout << endl;

	cout << SizeClass::RoundUp(129) << endl;
	cout << SizeClass::RoundUp(1023) << endl;
	cout << endl;

	cout << SizeClass::RoundUp(1025) << endl;
	cout << SizeClass::RoundUp(8*1024 - 1) << endl;
	cout << endl;

	cout << SizeClass::RoundUp(8 * 1024 + 1) << endl;
	cout << SizeClass::RoundUp(64 * 1024 - 1) << endl;
	cout << endl;

	cout << SizeClass::ListIndex(1) << endl;
	cout << SizeClass::ListIndex(128) << endl;
	cout << endl;

	cout << SizeClass::ListIndex(129) << endl;
	cout << SizeClass::ListIndex(1023) << endl;
	cout << endl;

	cout << SizeClass::ListIndex(1025) << endl;
	cout << SizeClass::ListIndex(8 * 1024 - 1) << endl;
	cout << endl;
	
	cout << SizeClass::ListIndex(8 * 1024 + 1) << endl;
	cout << SizeClass::ListIndex(64 * 1024 - 1) << endl;
	cout << endl;
}

int main()
{
	//UnitThreadCache();
	//UnitTestSizeClass();
	UnitTestRoundUp();
	return 0;
}
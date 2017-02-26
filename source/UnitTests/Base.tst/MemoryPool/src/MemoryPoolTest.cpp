/**
 * Memory.cpp
 * Created on: Wed Feb 25 16:40:00 IST 2017
 * @author: obhi
 */
#include <BaseHeaders.h>

NEX_IMPLEMENT_PLATFORM_APP()

using namespace nextar;

template <const nextar::MemoryCategory MemCat>
void MakeTest(nextar::uint32 o, nextar::uint32 num) {
	nextar::Pool<MemCat> pool(o, num);
	{
		nextar::uint8* data1 = static_cast<nextar::uint8*>(pool.Alloc());
		std::fill(data1, data1 + o, 1);
		pool.Free(data1);
	}
	{
		nextar::uint8* data1 = static_cast<nextar::uint8*>(pool.Alloc(num / 2));
		std::fill(data1, data1 + o*(num / 2), 1);
		nextar::uint8* data2 = static_cast<nextar::uint8*>(pool.Alloc(num / 2));
		std::fill(data2, data2 + o*(num / 2), 1);
		pool.Free(data1);
		pool.Free(data2);
	}
	{
		nextar::uint8* data1 = static_cast<nextar::uint8*>(pool.Alloc(num / 2));
		std::fill(data1, data1 + o*(num / 2), 1);
		pool.Free(data1);
		nextar::uint8* data2 = static_cast<nextar::uint8*>(pool.Alloc(num / 2));
		std::fill(data2, data2 + o*(num / 2), 1);
		pool.Free(data2);
	}
	{
		for (uint32 i = 0; i < 100; ++i) {
			nextar::uint8* data1 = static_cast<nextar::uint8*>(pool.Alloc(num * 2));
			std::fill(data1, data1 + o*(num / 2), 1);
			pool.Free(data1);
			nextar::uint8* data2 = static_cast<nextar::uint8*>(pool.Alloc(num / 2));
			std::fill(data2, data2 + o*(num / 2), 1);
			pool.Free(data2);
		}
	}
	{
		for (uint32 i = 0; i < 100; ++i) {
			nextar::uint8* data1 = static_cast<nextar::uint8*>(pool.Alloc());
			std::fill(data1, data1 + o, 1);
			nextar::uint8* data2 = static_cast<nextar::uint8*>(pool.Alloc());
			std::fill(data1, data1 + o, 1);
			pool.Free(data2);
			pool.Free(data1);
		}
	}
}

int NextarMain(int argc, char* argv[]) {
	nextar::ACBaseImpl impl;
	nextar::ApplicationContext application("AppContext", impl);

	application.InitializeContext(argc, argv);
	
	for (uint32 i = 0; i < 100; ++i) {
		MakeTest<MemoryCategory::MEMCAT_GENERAL>(
			Math::RandomFast::RangeLong(sizeof(ptrdiff_t), sizeof(ptrdiff_t) * 5),
			Math::RandomFast::RangeLong(4, 100));
	}
	for (uint32 i = 0; i < 100; ++i) {
		MakeTest<MemoryCategory::MEMCAT_CACHEALIGNED>(
			Math::RandomFast::RangeLong(sizeof(ptrdiff_t), sizeof(ptrdiff_t) * 5),
			Math::RandomFast::RangeLong(4, 100));
	}


	application.DestroyContext();
	return 0;
}

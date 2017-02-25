
#include <NexBase.h>
#include <MemoryManager.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(MemoryManager);


template class StaticMemoryPool<MEMCAT_GENERAL>;
template class StaticMemoryPool<MEMCAT_CACHEALIGNED>;
template class StaticMemoryPool<MEMCAT_SIMDALIGNED>;
template class StaticMemoryPool<MEMCAT_MATH_CORE>;
template class StaticMemoryPool<MEMCAT_BUFFER_DATA>;

MemoryManager MemoryManager::_instance;

template <const MemoryCategory MemCat>
class StaticMemoryPoolImpl : public StaticMemoryPool<MemCat> {
public:
	StaticMemoryPoolImpl(uint32 objectSize, uint32 numObjects) : StaticMemoryPool<MemCat>(objectSize, numObjects) {

	}

	virtual uint32 GetObjectSize() const {
		return _pool.GetChunkSize();
	}

	virtual uint32 GetDefaultNumObjectsPerBlock() const {
		return _pool.GetDefaultNumChunksPerBlock();
	}

	virtual MemoryCategory GetMemoryCatagory() const {
		return MemCat;
	}

	virtual void Destroy() {
		_pool.FreePool();
	}
};

MemoryManager::MemoryManager() {
}

MemoryManager::~MemoryManager() {
	ReleaseEverything();
}

void MemoryManager::ReleaseEverything() {
	
	auto it = memPools.begin();
	while (it != memPools.end()) {
		(*it).second->Destroy();
		NEX_DELETE((*it).second);
		++it;
	}

	MemoryPoolMap empty;
	std::swap(memPools, empty);
}

StaticMemoryPoolBase* MemoryManager::AcquirePoolInstance(MemoryCategory memCat, 
	uint32 objectSize, uint32 numPerBlock) {
	switch (memCat) {
	case MEMCAT_GENERAL:
	case MEMCAT_SIMDALIGNED:
	case MEMCAT_MATH_CORE:
	case MEMCAT_BUFFER_DATA:
	case MEMCAT_CACHEALIGNED:
		break;
	default:
		memCat = MEMCAT_CACHEALIGNED;
		break;
	}

	uint64 id = GetPoolID(objectSize, numPerBlock, memCat);
	auto it = memPools.find(id);
	if (it != memPools.end()) {
		return (*it).second;
	}
	
	StaticMemoryPoolBase* ret = nullptr;
	switch (memCat)	{
	case MEMCAT_GENERAL:
		ret = NEX_NEW(StaticMemoryPoolImpl<MEMCAT_GENERAL>(objectSize, numPerBlock));
		break;
	case MEMCAT_SIMDALIGNED:
		ret = NEX_NEW(StaticMemoryPoolImpl<MEMCAT_SIMDALIGNED>(objectSize, numPerBlock));
		break;
	case MEMCAT_MATH_CORE:
		ret = NEX_NEW(StaticMemoryPoolImpl<MEMCAT_MATH_CORE>(objectSize, numPerBlock));
		break;
	case MEMCAT_BUFFER_DATA:
		ret = NEX_NEW(StaticMemoryPoolImpl<MEMCAT_BUFFER_DATA>(objectSize, numPerBlock));
		break;
	case MEMCAT_CACHEALIGNED:
	default:
		ret = NEX_NEW(StaticMemoryPoolImpl<MEMCAT_CACHEALIGNED>(objectSize, numPerBlock));
		break;
	}

	memPools[id] = ret;
	return ret;
}

void MemoryManager::ReleasePoolInstance(StaticMemoryPoolBase * memPool) {
	uint64 id = GetPoolID(memPool->GetObjectSize(), 
		memPool->GetDefaultNumObjectsPerBlock(), memPool->GetMemoryCatagory());
	auto it = memPools.find(id);
	if ((*it).second) {
		(*it).second->Destroy();
		NEX_DELETE((*it).second);
	}
	memPools.erase(it);
}

}
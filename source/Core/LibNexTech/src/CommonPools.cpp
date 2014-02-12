
#include "NexHeaders.h"
#include "MathBase.h"
#include "TransformData.h"
#include "BoundingVolume.h"
#include "CommonPools.h"

namespace nextar {

	template class SingletonPool<Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore>;
	template class SingletonPool<TransformData, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore>;
	template class SingletonPool<BoundingBox, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore>;
	
	void PoolManager::CreateCommonPools() {
		Matrix4x4Pool::AllocPool();
		TransformDataPool::AllocPool();
		BoundingBoxPool::AllocPool();
	}

	void PoolManager::DestroyCommonPools() {
		Matrix4x4Pool::FreePool();
		TransformDataPool::FreePool();
		BoundingBoxPool::FreePool();
	}
}
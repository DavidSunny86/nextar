/*
 * Main.cpp
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <TaskSchedular.h>

namespace nextar {



}

NEX_IMPLEMENT_PLATFORM_APP();

using namespace nextar;

static unsigned MAX_DEPTH = 20;
Matrix4x4 RandomMatrix() {
	Matrix4x4 temp;
	for(uint32 i = 0; i < 4; ++i)
		for(uint32 j = 0; j < 4; ++j)
			temp(i,j) = RandomGen::RangeFloat(0, Math::PI);
	return temp;
}

class SubTask : public Task {
public:
	SubTask(const String& name, uint32 depth) : Task(name) {
		result = NEX_NEW(Matrix4x4);
		*result = Matrix4x4::IdentityMatrix;
		next = nullptr;
		this->depth = depth;
	}
	~SubTask() {
		if (next)
			NEX_DELETE(next);
	}

	virtual Task* Run() {
		RandomGen::Seed(std::time(nullptr));

		for(unsigned i = 1; i < count; ++i)
			*result = Mat4x4Mul(*result, RandomMatrix());

		if (depth < MAX_DEPTH) {
			next = NEX_NEW(SubTask(_name + "->st" + Convert::ToString((int32)depth+1), depth+1));
			next->count = (count*2)/3;
			if (depth%2)
				return next;
			else
				Spawn(next);
		}
		WaitForChildren();
		return nullptr;
	}

	uint32 depth;

	Matrix4x4* result;
	uint32 count;
protected:
	SubTask* next;
};

class MyTask : public Task {
public:
	MyTask() : Task("MyTask") {
		for(int i = 0; i < 3; ++i)
			task[i] = NEX_NEW( SubTask( String("t") + Convert::ToString((int32)i), 1 ) );;
	}

	~MyTask() {
		for(int i = 0; i < 3; ++i)
			NEX_DELETE(task[i]);
	}

	virtual Task* Run() {

		uint32 count = 1000;
		// matrix multiply 1000 times
		task[0]->count = count;
		task[1]->count = count;
		task[2]->count = count;

		Spawn(task[0]);
		Spawn(task[1]);
		Spawn(task[2]);

		WaitForChildren();
		return nullptr;
	}

protected:
	SubTask* task[3];
};

int NextarMain(int argc, char* argv[]) {
	nextar::ApplicationContext application("FileSystem");

	application.InitializeContext(argc, argv);
	TaskSchedular* schedular = NEX_NEW(TaskSchedular());
	MyTask task;
	schedular->AsyncAddChildTask(&task);
	task.Wait();
	NEX_DELETE(schedular);
	application.DestroyContext();

	std::cin.get();
	return 0;
}

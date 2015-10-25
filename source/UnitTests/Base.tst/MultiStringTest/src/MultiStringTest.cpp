/**
 * MultiStringTest.cpp
 * Created on: Wed, Oct 21, 2015  8:32:35 PM
 * @author: obhi
 */
#include <UTDefs.h>
#include <BaseHeaders.h>

NEX_IMPLEMENT_PLATFORM_APP()
;
using namespace nextar;


void TestPushBackAndIteration() {
	String Numbers[5] = {
		String("One"),
		String("Two"),
		String("Three"),
		String("Four"),
		String("Five"),
	};

	String myString;
	MultiStringHelper h(myString);
	for (uint32 i = 0; i < 5; ++i) {
		h.PushBack(Numbers[i]);
	}

	MultiStringHelper::Iterator it = h.Iterate();
	String value;
	uint32 i = 0;
	while (it.HasNext(value)) {
		NEX_UT_ASSERT(i < 5);
		NEX_UT_ASSERT(value == Numbers[i++]);
	}

	String One("One");
	MultiStringHelper::Iterator it2 = h.IteratorOf(One);
	NEX_UT_ASSERT(it2 == h.Iterate());
	NEX_UT_ASSERT(h.IndexOf(Numbers[3]) == 3);
	NEX_UT_ASSERT(h.Get(3) == Numbers[3]);
	String frontNumbers[5] = {
		String("FOne"),
		String("FTwo"),
		String("FThree"),
		String("FFour"),
		String("FFive"),
	};
	for (uint32 i = 0; i < 5; ++i) {
		h.PushFront(frontNumbers[i]);
	}
	
	it = h.Iterate();
	i = 0;
	while (it.HasNext(value)) {
		if(i < 5)
			NEX_UT_ASSERT(value == frontNumbers[5-(++i)]);
	}
	NEX_UT_ASSERT(h.Length() == 10);
}

void TestStringsAPI(nextar::ApplicationContext& c) {
	TestPushBackAndIteration();
}

int NextarMain(int argc, char* argv[]) {
	nextar::ApplicationContext application("MultiStringTest");
	application.InitializeContext(argc, argv);
	TestStringsAPI(application);
	application.DestroyContext();
	std::cin.get();
	return 0;
}

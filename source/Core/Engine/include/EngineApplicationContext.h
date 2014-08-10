#ifndef ENGINEAPPLICATIONCONTEXT_H_
#define ENGINEAPPLICATIONCONTEXT_H_

#include <NexEngine.h>
#include <ApplicationContext.h>

namespace nextar {

class _NexEngineAPI EngineApplicationContext: public ApplicationContext {
	NEX_LOG_HELPER(ApplicationContext)
	;

public:

	enum EventIDs {
		EVENT_RENDERMANAGER_PRE_CLOSE = ApplicationContext::LAST_EVENT_ID + 1,
		EVENT_RENDERMANAGER_POST_CLOSE,
		LAST_EVENT_ID,
	};

	EngineApplicationContext(const String& name);
	virtual ~EngineApplicationContext(void);

	virtual void ReleaseResourcesImpl();

protected:
		
	virtual void ConfigureExtendedInterfacesImpl();
	virtual void CreateExtendedInterfacesImpl();
	virtual void DestroyExtendedInterfacesImpl();

private:
};

}

#endif //NEX_APPLICATIONCONTEXT_H_
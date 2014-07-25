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
		EVENT_RENDERMANAGER_PRE_CLOSE,
		EVENT_RENDERMANAGER_POST_CLOSE
	};

	EngineApplicationContext(const String& name);
	virtual ~EngineApplicationContext(void);

protected:
	
	virtual void ReleaseResources() = 0;
	virtual void ConfigureExtendedInterfacesImpl();
	virtual void CreateExtendedInterfacesImpl();
	virtual void DestroyExtendedInterfacesImpl();

private:
};

}

#endif //NEX_APPLICATIONCONTEXT_H_
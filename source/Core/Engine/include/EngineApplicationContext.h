#ifndef ENGINEAPPLICATIONCONTEXT_H_
#define ENGINEAPPLICATIONCONTEXT_H_

#include <NexEngine.h>
#include <ApplicationContext.h>

namespace nextar {

class _NexEngineAPI EngineApplicationContext: public ACBaseImpl {
	NEX_LOG_HELPER(ApplicationContext)
	;

public:

	enum EventIDs {
		EVENT_RENDERMANAGER_CREATED = ApplicationContext::LAST_EVENT_ID + 1,
		EVENT_RENDERMANAGER_PRE_CLOSE,
		EVENT_RENDERMANAGER_POST_CLOSE,
		LAST_EVENT_ID,
	};

	EngineApplicationContext();
	virtual ~EngineApplicationContext(void);

	virtual void ReleaseResources();

	ApplicationContextType GetType() const {
		return ApplicationContextType(CONTEXT_ENGINE);
	}

protected:
		
	virtual void ConfigureExtendedInterfacesImpl(Config& config) override;
	virtual void CreateExtendedInterfacesImpl() override;
	virtual void DestroyExtendedInterfacesImpl() override;

private:
};

}

#endif //NEX_APPLICATIONCONTEXT_H_
/*
 * ProjectApplicationContext.h
 *
 *  Created on: 20-Jul-2014
 *      Author: obhi
 */

#ifndef PROJECTAPPLICATIONCONTEXT_H_
#define PROJECTAPPLICATIONCONTEXT_H_

#include <NexProjectModel.h>
#include <EngineApplicationContext.h>

namespace nextar {

/*
 *
 */
class _NexProjectAPI ProjectApplicationContext: public EngineApplicationContext {
public:
	enum EventIDs {
		LAST_EVENT_ID = ApplicationContext::LAST_EVENT_ID + 1,
	};

	ProjectApplicationContext();
	virtual ~ProjectApplicationContext();

	ApplicationContextType GetType() const {
		return ApplicationContextType(CONTEXT_PROJECT);
	}

protected:
	virtual void CreateExtendedInterfacesImpl();
	virtual void ReleaseResources();

	void RegisterTemplateFactories();
};

} /* namespace nextar */

#endif /* PROJECTAPPLICATIONCONTEXT_H_ */

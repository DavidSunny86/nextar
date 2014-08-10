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

	ProjectApplicationContext(const String& name);
	virtual ~ProjectApplicationContext();

protected:
	virtual void CreateExtendedInterfacesImpl();
	virtual void ReleaseResourcesImpl();

	void RegisterTemplateFactories();
};

} /* namespace nextar */

#endif /* PROJECTAPPLICATIONCONTEXT_H_ */

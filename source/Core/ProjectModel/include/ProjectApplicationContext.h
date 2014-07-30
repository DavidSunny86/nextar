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
	ProjectApplicationContext(const String& name);
	virtual ~ProjectApplicationContext();

protected:
	virtual void CreateExtendedInterfacesImpl();
	virtual void ReleaseResources();

	void RegisterTemplateFactories();
};

} /* namespace nextar */

#endif /* PROJECTAPPLICATIONCONTEXT_H_ */

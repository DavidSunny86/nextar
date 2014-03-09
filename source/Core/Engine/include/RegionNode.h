/* 
 * File:   Region.h
 * Author: obhi
 *
 * Created on February 19, 2011, 9:06 AM
 */

#ifndef REGIONNODE_H
#define	REGIONNODE_H

#include <scene/ContainerNode.h>
#include <scene/LightNode.h>

namespace nextar {

	class _NexEngineExport RegionNode: public ContainerNode {
	public:
		enum {
			TYPE = COMPONENT_REGION_NODE,
		};

		RegionNode(ComponentManager *creator,
				const String& name = StringUtils::Null, bool allocMatrixBuff =
						true);
		virtual ~RegionNode();

		virtual int GetType() const {
			return TYPE;
		}

		/** Add child component */
		virtual void AddChild(Movable*);
		/** Remove child component */
		virtual void RemoveChild(Movable*);

		virtual void NotifyNodeAdded(Movable*);
		virtual void NotifyNodeRemoved(Movable*);

	protected:
		CullingSystem* cullingSystem;
	};

}

#endif	/* REGIONNODE_H */


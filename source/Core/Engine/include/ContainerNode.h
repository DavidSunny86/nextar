#ifndef CONTAINERNODE_H
#define CONTAINERNODE_H

#include "scene/Node.h"

namespace nextar {

	/**
	 * @addtogroup Scene
	 * @{
	 * @par Region
	 * @par Regions are mostly static constructs storing a collection of Nodes.
	 *      Though they derive from Node, classic Regions do not use
	 *      matrices, because they are not representing any hierarchy.
	 *      The matrix buffer is thus kept NULL, if not otherwise required. This
	 *      behaviour can be overriden by some Region types which wish to be
	 *      dynamic in nature.
	 *      The GenRegion implementation is an example of a generic non-classic
	 *      region.
	 * @par
	 *      Regions are connected in space through Portals.
	 * @}
	 */
	class _NexEngineExport ContainerNode: public Movable {
	protected:

		NodeList children;
		NodeList childUpdates;

	public:
		ContainerNode(ComponentManager *creator,
				const String& name = StringUtils::Null, bool allocMatrixBuff =
						true);
		~ContainerNode();

		/** @copydoc Node::AddChild */
		virtual void AddChild(Movable*);
		/** @copydoc Node::RemoveChild */
		virtual void RemoveChild(Movable*);

		/** @remarks Update world volume */
		virtual void UpdateBounds();

		/** @copydoc Node::NotifyUpdated */
		virtual void NotifyUpdated(bool forceNotifyParent = false);
		/** @brief Called by child to inform parent that child needs update */
		virtual void NotifyChildNeedsUpdate(Movable* child);
		/** @brief update */
		virtual void Update(bool parentChanged);
		/** @brief Visit renderables attached to this node **/
		virtual void FindVisibles(SceneTraversal& traversal);
		/** @brief Find a child component */
		/*virtual Component* AsyncFindChild(const String& name);*/
	};

}
#endif //CONTAINERNODE_H

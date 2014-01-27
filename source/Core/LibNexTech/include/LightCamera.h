/* 
 * File:   LightCamera.h
 * Author: obhi
 *
 * Created on June 17, 2011, 10:10 AM
 */

#ifndef LIGHTCAMERA_H
#define	LIGHTCAMERA_H

#include "Camera.h"
#include "BoundingVolume.h"

namespace nextar {

	class LightCamera: public Camera, public AllocScene {
	public:
		LightCamera(RegionNode* region);
		virtual ~LightCamera();

		void SetView(LightNode* node);
		void AdjustProjection(const BoundingVolume* sceneVolume) {
		}

		void SetRenderCam(Camera* renderCam) {
			this->renderCam = renderCam;
		}

		Camera* GetRenderCam() const {
			return renderCam;
		}

		/** @todo Make use of this?? */
		virtual void FindVisiblePrimitives(SceneTraversal & traversal) {}

	private:

		RegionNode* parentRegion;
		Camera* renderCam;
	};
}

#endif	/* LIGHTCAMERA_H */


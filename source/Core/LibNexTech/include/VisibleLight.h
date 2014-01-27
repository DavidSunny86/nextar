/*
 * VisibleLight.h
 *
 *  Created on: 25-Nov-2013
 *      Author: obhi
 */

#ifndef VISIBLELIGHT_H_
#define VISIBLELIGHT_H_

#include "NexSys.h"
#include "Color.h"
#include "NexMath.h"


namespace nextar {

	class VisibleLight {
	public:

		VisibleLight(Type);
		virtual ~VisibleLight();

		Shader* GetShader() const {
			return appliedShader;
		}

		/** Volumetric lighting may use stream data */
		StreamData* GetLightVolume() const {
			return lightVolume;
		}

		void SetLightType(Type lightType) {
			this->lightType = lightType;
		}

		Type GetLightType() const {
			return lightType;
		}

		bool IsScissorRectEnabled() const {
			return scissorRectEnabled;
		}

		const Vector3A* GetWorldPosition() const {
			return worldPosition;
		}

		const Vector3A* GetWorldDirection() const {
			return worldDirection;
		}


		void SetAmbientColor(float red, float green, float blue) {
			ambientColor.red = red;
			ambientColor.green = green;
			ambientColor.blue = blue;
		}

		void SetDiffuseColor(float red, float green, float blue) {
			diffuseColor.red = red;
			diffuseColor.green = green;
			diffuseColor.blue = blue;
		}

		void SetSpecularColor(float red, float green, float blue) {
			specularColor.red = red;
			specularColor.green = green;
			specularColor.blue = blue;
		}

		const Color& GetAmbientColor() const {
			return ambientColor;
		}

		const Color& GetDiffuseColor() const {
			return diffuseColor;
		}

		const Color& GetSpecularColor() const {
			return specularColor;
		}

		void SetAttenCoefficients(float constant, float linear, float quadratic) {
			attenConstant = constant;
			attenLinear = linear;
			attenQuadratic = quadratic;
		}

		void SetAttenQuadratic(float attenQuadratic) {
			this->attenQuadratic = attenQuadratic;
		}

		float GetAttenQuadratic() const {
			return attenQuadratic;
		}

		void SetAttenLinear(float attenLinear) {
			this->attenLinear = attenLinear;
		}

		float GetAttenLinear() const {
			return attenLinear;
		}

		void SetAttenConstant(float attenConstant) {
			this->attenConstant = attenConstant;
		}

		float GetAttenConstant() const {
			return attenConstant;
		}

		void SetAttenRange(float attenRange) {
			this->attenRange = attenRange;
		}

		float GetAttenRange() const {
			return attenRange;
		}

		void SetFalloffAngle(float falloffAngle) {
			this->falloffAngle = falloffAngle;
		}

		float GetFalloffAngle() const {
			return falloffAngle;
		}

		const float* GetAttenuation() const {
			return attenuation;
		}

	protected:
		bool scissorRectEnabled;

		/* Fall off angle for spot lights */
		float falloffAngle;

		union {
			struct {
				float attenRange;
				float attenConstant;
				float attenLinear;
				float attenQuadratic;
			};

			float attenuation[4];
		};

		Vector3A* worldPosition;
		Vector3A* worldDirection;

		Type lightType;
		StreamData* lightVolume;
		Shader* appliedShader;
	};

}


#endif /* VISIBLELIGHT_H_ */

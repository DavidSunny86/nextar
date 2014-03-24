/*
 * ParameterIterator.h
 *
 *  Created on: 14-Mar-2014
 *      Author: obhi
 */

#ifndef PARAMETERITERATOR_H_
#define PARAMETERITERATOR_H_

#include <ConstantBuffer.h>
#include <Pass.h>

namespace nextar {

	/*
	 *
	 */
	// Iterator for custom parameters supported by this shader.
	// Iterator is specific to the type of paramters requested.
	class _NexEngineAPI ParameterIterator : public AllocGraphics {
	public:
		ParameterIterator(PassList& passes, uint32 type);
		ParameterIterator(const ParameterIterator& it);

		operator bool();
		const ShaderParamDesc& operator *() const;
		ParameterIterator& operator ++();

		inline void Reset() {
			_Init();
		}
	protected:

		void _Init();
		void _NextParam();
		PassList::iterator curPass;
		ConstantBufferList::iterator curBuffer;
		ShaderParamIterator curParam;
		uint32 type;

		PassList& passes;

	private:
		ParameterIterator& operator = (const ParameterIterator& it);
	};

} /* namespace nextar */

#endif /* PARAMETERITERATOR_H_ */

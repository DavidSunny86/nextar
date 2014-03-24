/*
 * ParameterIterator.cpp
 *
 *  Created on: 14-Mar-2014
 *      Author: obhi
 */

#include <ParameterIterator.h>

namespace nextar {

	/*****************************************************/
	/* ParameterIterator             			 */
	/*****************************************************/
	ParameterIterator::ParameterIterator(PassList& _passes, uint32 _type) :
	passes(_passes), type(_type) {
		_Init();
	}

	ParameterIterator::ParameterIterator(const ParameterIterator& it) :
	passes(it.passes), type(it.type) {
		_Init();
	}

	void ParameterIterator::_Init() {
		curPass = passes.begin();
		while (curPass != passes.end()) {
			ConstantBufferList& cbl = (*curPass)->GetConstantBuffers();
			curBuffer = cbl.begin();
			while (curBuffer != cbl.end()) {
				curParam = (*curBuffer)->GetParamIterator();
				_NextParam();
				if (curParam)
					break;
				++curBuffer;
			}
			if(!curParam) {
				curParam = (*curPass)->GetSamplerIterator();
				_NextParam();
			}

			if (curParam)
				break;
			++curPass;
		}
	}

	void ParameterIterator::_NextParam() {
		while(curParam) {
			if ((*curParam).frequency & type)
				break;
			++curParam;
		}
	}

	ParameterIterator::operator bool() {
		return curParam;
	}

	ParameterIterator& ParameterIterator::operator ++() {
		if (curParam) {
			// look for the next from the current list
			_NextParam();
			if (!curParam) {

				ConstantBufferList& cbl = (*curPass)->GetConstantBuffers();
				if (curBuffer != cbl.end()) {
					// go to the next cb
					++curBuffer;
				start_over:
					while (curBuffer != cbl.end()) {
						curParam = (*curBuffer)->GetParamIterator();
						_NextParam();
						if (curParam)
							break;
						++curBuffer;
					}
					// we went past the last cb, try samplers
					if (!curParam) {
						curParam = (*curPass)->GetSamplerIterator();
						_NextParam();
					}
				}
				// no cb or samplers found, next pass
				if (!curParam) {
					++curPass;
					if (curPass != passes.end()) {
						ConstantBufferList& cbl = (*curPass)->GetConstantBuffers();
						curBuffer = (*curPass)->GetConstantBuffers().begin();
						goto start_over;
					}
				}
			}
		}
		return *this;
	}
} /* namespace nextar */

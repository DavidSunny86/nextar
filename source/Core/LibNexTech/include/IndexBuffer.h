/* 
 * File:   IndexBuffer.h
 * Author: obhi
 *
 * Created on February 6, 2011, 10:23 PM
 */

#ifndef INDEXBUFFER_H
#define	INDEXBUFFER_H

#include "ManagedBuffer.h"

namespace nextar {

    class IndexBuffer : public ManagedBuffer {
    public:

        enum Type {
            TYPE_32BIT,
            TYPE_16BIT
        };
        typedef uint8 TypeTag;

        IndexBuffer(Type type) :  index_type(static_cast<TypeTag>(type)) {
        }
        virtual ~IndexBuffer();

        Type GetIndexType() const {
            return static_cast<Type>(index_type);
        }
       
    private:

        TypeTag index_type;
    };
}

#endif	/* INDEXBUFFER_H */



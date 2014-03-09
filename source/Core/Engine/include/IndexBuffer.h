/* 
 * File:   IndexBuffer.h
 * Author: obhi
 *
 * Created on February 6, 2011, 10:23 PM
 */

#ifndef INDEXBUFFER_H
#define	INDEXBUFFER_H

#include <ManagedBuffer.h>

namespace nextar {

    class IndexBuffer : public ManagedBuffer {
    public:

        enum Type {
            TYPE_32BIT,
            TYPE_16BIT
        };
        typedef uint8 TypeTag;

        inline IndexBuffer(size_t size, uint32 flags, Type type) :  ManagedBuffer(size, flags), indexType(static_cast<TypeTag>(type)) {
        }
        virtual ~IndexBuffer();

        Type GetIndexType() const {
            return static_cast<Type>(indexType);
        }
       
    private:

        TypeTag indexType;
    };
}

#endif	/* INDEXBUFFER_H */



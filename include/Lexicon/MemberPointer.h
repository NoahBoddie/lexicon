#pragma once

#include "TypeID.h"

namespace LEX
{
	/*
	MemberPointers will need a change, along with an addition to runtime. When trying to access a member, 
	there are times you'll need to access a member from a specific type. When this is relevant, 
	the field type will be loaded into the runtime and then a call will be made on the member pointer. 
	It will target this specific one, not using the shared inherited type, but the one specifically owned by that type.

	There can only be one type that derives from that type internally and still visible by this point.
	//*/


	


	//TODO: Member pointer should PORBABLY be engine. Also, I'd like to use bitfields to allow for a virtual mode and non-virtual mode.
	// Maybe will use unions to help with that

    ENUM(MemberFlag, uint8_t)
    {
        None,
        Generic = 1 << 0,   //Used to tell if member needs to be moved when fully specialized. Incapable of being referenced otherwise.
        Partial = 1 << 1,   //A partialized type that will need to specialize in order to lose the flag.
    };


    //TODO: Member pointer should PORBABLY be engine. Also, I'd like to use bitfields to allow for a virtual mode and non-virtual mode.
    // Maybe will use unions to help with that
    struct MemberPointer
    {
        //May remake, instead using member or method info in order to handle this sort of thing.
        // Or I'll just store this as a pointer.

        //OR, scripted data types can only hold 16 bit spec codes, and intrinsics can hold 32 bit ones.

        //Used to get a method or member, and correcting the intended index.


        union
        {
            uint32_t raw;
            TypeID typeID;
            InstanceID instanceID;
        };

        MemberFlag flags = MemberFlag::None;

        uint16_t index = 0;


        MemberPointer Resolve(ITemplateBody*& body)
        {
            auto result = *this;

            result.flags = {};

            return result;
        }

        //Should have an option to ctor who it came from.

        constexpr operator MemberHash() const
        {
            return std::bit_cast<MemberHash>(*this);
        }
    };
    static_assert(sizeof(MemberPointer) == 0x8);



}
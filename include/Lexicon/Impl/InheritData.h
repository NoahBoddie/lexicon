#pragma once

#include "AccessModifier.h"

namespace LEX
{

	struct ITypePolicy;

	struct InheritData
	{
		//The hash is a value that represents memory wise, where said object can be considered within memory.
		std::array<uint32_t, 2> hash{};


		//Distance is -1 if virtually inherited
		uint32_t distance = 0;
		ITypePolicy* type = nullptr;

		uint32_t _id = 0;

		//This can possibly be unionized, with the sign bit being able to tell if one or the other.
		uint32_t ownerIndex = 0;

		uint32_t memberIndex = 0;

		Access access;//
		//I think I'll store internal access outside of this.

		bool isGeneric = false;//This differs if the id is an instance id or a type id.
		bool virtInherited = false;

		constexpr bool operator<(const InheritData& other) const
		{
			if (isGeneric != other.isGeneric)
				return !isGeneric < !other.isGeneric;

			return  distance < other.distance;
		}

		//Need to figure out how to move these.

		//These are deprecated, as methods
		//uint32_t memberRange [2];//This should be where this entries stuff starts, and then where it ends. same deal with the other.
		//uint32_t methodRange [2];

		Access GetAccess() const
		{
			return access & ~Access::Internal;
		}

		void SetAccess(Access a_acc)
		{
			auto pop = access & Access::Internal;
			a_acc &= ~Access::Internal;
			access = a_acc;
			access |= pop;
		}

		//I want to merge a bunch of flags into where access is. Stuff like is generic or is internal, virtual inherited all can probably be shoved in there.

		void SetInternal(bool v)
		{
			if (v) {
				access |= Access::Internal;
			}
			else {
				access &= ~Access::Internal;
			}
		}

		bool IsInternal() const
		{
			return access & Access::Internal;
		}

		bool IsVirtualInherited() const
		{
			return virtInherited;
		}

	};


}
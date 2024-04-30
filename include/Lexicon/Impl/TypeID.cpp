#pragma once

#include "TypeID.h"

#include "IdentityManager.h"

namespace LEX
{



		TypeID TypeID::CreateTrival(uint16_t code)
		{
			//Makes a Trival
			return CreateID(0);//IdentityManager::GetIDFromOffset("Trival", code);

		}

		TypeOffset TypeID::GetOffset() const
		{
			return CreateID(0);//return IdentityManager::GetOffsetFromID(*this);
		}


		/*

		operator uint32_t()
		{
			return _value;
		}

		bool IsTrival() const
		{
			return std::clamp<uint32_t>(_value, Trival::trivalStart, Trival::trivalEnd) == _value;
		}

		constexpr uint8_t GetIndex() const
		{
			return ;
		}


		constexpr uint32_t GetID() const
		{
			//Should clear the presence
			return 0;
		}


		auto operator<=>(const TypeID& other) const
		{
			return _value <=> other._value;
		}
		//*/

}

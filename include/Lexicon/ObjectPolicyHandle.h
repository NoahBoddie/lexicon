#pragma once

#include "Lexicon/Interfaces/InterfaceManager.h"

namespace LEX
{
	struct ObjectPolicy;

	struct ObjectPolicyHandle
	{
		static constexpr uint32_t invalid_index = -1;

	public:
		constexpr ObjectPolicyHandle() noexcept = default;

		ObjectPolicyHandle(uint32_t i) : _index{ i }
		{
			if (IsValid() == true)
				InterfaceManager::ModInterfaceUseCount(true);
		}

		//The constructors need the permission of the other to start incrementing
		ObjectPolicyHandle(const ObjectPolicyHandle& other) : ObjectPolicyHandle{ other._index }{}

		~ObjectPolicyHandle()
		{
			if (IsValid() == true)
				InterfaceManager::ModInterfaceUseCount(false);
		}






		ObjectPolicyHandle& operator=(uint32_t i)
		{
			bool was_valid = IsValid();

			_index = i;

			if (IsValid() != was_valid)
				InterfaceManager::ModInterfaceUseCount(!was_valid);

			return *this;
		}



		//Assignment needs to check if it already was first.
		ObjectPolicyHandle& operator=(const ObjectPolicyHandle& other)
		{
			return operator=(other._index);
		}





		ObjectPolicy* get() const;


		operator ObjectPolicy* () const
		{
			return get();

		}


		ObjectPolicy* operator-> ()const
		{
			return get();
		}

		constexpr bool IsValid() const noexcept
		{
			return _index != invalid_index;
		}
		constexpr operator bool() const noexcept
		{
			return IsValid();
		}

		//Expand to 3 way operator.
		//bool operator ==(ObjectPolicyHandle other)
		//{
		//	return _index == other._index;
		//}

		//constexpr std::strong_ordering operator <=>(ObjectPolicyHandle other) const noexcept
		//{
		//	return _index <=> other._index;
		//}

		constexpr std::strong_ordering operator <=>(const ObjectPolicyHandle& other) const noexcept = default;
		//Sort out these ops real quick.

		uint32_t index() const
		{

			return _index;
		}

	private:

		uint32_t _index = -1;

	};
	REQUIRED_SIZE(ObjectPolicyHandle, 0x4);


	//Get ObjectPolicyHandle Here.
}
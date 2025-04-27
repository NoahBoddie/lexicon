#pragma once

#include "Lexicon/Reflect.h"

namespace LEX
{
	struct TypeInfo;


	struct Reflection
	{
		static TypeInfo* GetVariableType(const Reflection* reflect);


		virtual Reflect GetReflect() const = 0;
		virtual size_t GetReflectOffset() const = 0;
		virtual bool IsValidOffset(size_t offset) const
		{
			//This can be used for abstract types that may not update right.
			return GetReflectOffset() == offset;
		}


		//bool IsValidOffset(auto offset) const requires (std::is_enum_v<decltype(offset)>)
		//{
		//	IsValidOffset(static_cast<size_t>(offset));
		//}



		template<std::derived_from<Reflection> T>
		std::add_pointer_t<T> As(Reflect refl, size_t offset = 0)
		{
			//This will turn into the meta type. Then, it will be able to be cast into the given type
			// as long as the offsets match

			if (!this || GetReflect() != refl || IsValidOffset(offset) == false)
				return nullptr;

			return static_cast<std::add_pointer_t<T>>(this);
		}

		template<std::derived_from<Reflection> T, typename TEnum> requires (std::is_enum_v<TEnum>)
			std::add_pointer_t<T> As(Reflect refl, TEnum offset)
		{
			return As<T>(refl, static_cast<size_t>(offset));
		}
	};

}
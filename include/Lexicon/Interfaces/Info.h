#pragma once

#include "Lexicon/InfoType.h"
#include "Lexicon/Impl/InfoDetails.h"

namespace LEX
{
	namespace Version
	{
		namespace _1
		{
			struct M_INTERFACE_VERSION(Info)
			{
				virtual std::string_view GetName() const = 0;
				virtual InfoType GetInfoType() const = 0;
				virtual uint16_t GetInfoOffset() const = 0;

				virtual const void* Cast(const void* self, InfoType from, InfoType to) const = 0;

			};
		}

		CURRENT_VERSION(Info, 1);
	}


	struct IMPL_VERSION(Info)
	{
		DEFINE_INFO_OFFSET(InfoType::Info)
    private:
        const void* Cast(const void* self, InfoType from, InfoType to) const override INTERFACE_FUNCTION;
        void* Cast(const void* self, InfoType from, InfoType to) { return unconst(make_const(this)->Cast(self, from, to)); }

    
    
    public:
        template<typename T, typename Self, typename = std::enable_if_t<
            std::negation_v<
            std::disjunction<
            std::is_pointer<T>,
            std::is_reference<T>,
            std::is_const<T>,
            std::is_volatile<T>>>>>
            copy_cv_t<Self, T>* As(this Self & a_this, InfoType type)
        {
#define AS_INFO(mc_info) As<::LEX::mc_info>(::LEX::InfoType::mc_info)

            using To = copy_cv_t<Self, T>;
            if (Self* ptr = std::addressof(a_this)) {

                if constexpr (std::is_convertible_v<const std::remove_cvref_t<Self>*, const T*>)
                {
                    return static_cast<To*>(ptr);
                }
                else {


                    if constexpr (requires() { sizeof(T) > 0; }&& requires() { { T::INFO_TYPE } -> std::convertible_to<InfoType>; })
                    {
                        if (T::INFO_TYPE != type) {
                            report::fault::critical("Incorrect type submitted for casting, expected {}, recieved {}",
                                magic_enum::enum_name(T::INFO_TYPE), magic_enum::enum_name(type));
                        }
                    }


                    return reinterpret_cast<To*>(ptr->Cast(ptr, std::remove_cvref_t<Self>::INFO_TYPE, type));
                }
            }

            return nullptr;
        }




        template<typename T, typename Self, typename = std::enable_if_t<
            std::negation_v<
            std::disjunction<
            std::is_pointer<T>,
            std::is_reference<T>,
            std::is_const<T>,
            std::is_volatile<T>>>>>
            copy_cv_t<Self, T>* As(this Self& a_this) requires(requires() { { T::INFO_TYPE } -> std::convertible_to<InfoType>; })
        {
            return a_this.As<T>(T::INFO_TYPE);
        }

	};
}
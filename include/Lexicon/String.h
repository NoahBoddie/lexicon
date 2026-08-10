#pragma once


#include <string_view>

#include "Lexicon/VariableType.h"

//*src
#include "Lexicon/TypeInfo.h"
#include "Lexicon/Interfaces/IdentityManager.h"



namespace LEX
{
	struct TypeInfo;

	using c_string = const char*;

    struct String
    {
    private:
        struct SmallData
        {
            char    chars[15]{};
            int8_t  sizeByte{ 0 };
        };
        struct BigData
        {
            char* ptr = nullptr;
            ptrdiff_t size = 0;
        };

        static constexpr uint8_t SMALL_STRING_LENGTH = sizeof(SmallData::chars);

        constexpr static bool is_length_small(size_t size) noexcept
        {
            return SMALL_STRING_LENGTH >= size;
        }

    public:

        constexpr ~String() { clear(); }

        constexpr String() noexcept : _small{} {}

        template<size_t N>
        constexpr String(const char(&other)[N]) noexcept requires(N <= SMALL_STRING_LENGTH) {
            set_data(other);
        }

        template<size_t N>
        String(const char(&other)[N]) noexcept requires(N >= SMALL_STRING_LENGTH) {
            set_data(other);
        }



        //merge these into something like what string view does so it isn't shit with new strings.
        String(const std::string_view& view){
            set_data(view);
        }

        String(const std::string& str)
        {
            set_data(str);
        }

        template<typename Char>
        String(Char other) noexcept requires(std::is_pointer_v<Char>&& std::is_same_v<std::remove_const_t<std::remove_pointer_t<Char>>, char>) {
            set_data(other);
        }

        /*

        String(const char* str)
        {
            set_data(str);
        }

        

        //*/

        String(const String& other)
        {
            if (copy_small_string(other) == false) {
                set_data(other.view());
            }
        }

        String(String&& other)
        {
            if (move_small_string(other) == false) {
                _big.ptr = std::exchange(other._big.ptr, nullptr);
                _big.size = std::exchange(other._big.size, 0);
            }
        }


        explicit String(size_t length)
        {
            create_data(length);
            
            auto data = ptr();

            for (size_t i = 0; i < length; ++i) {
                data[i] = '\n';
            }
        }

        String& operator=(const char* str)
        {
            set_data(str);
            return *this;
        }

        String& operator=(std::string_view view)
        {
            set_data(view);
            return *this;
        }

        String& operator=(std::string str)
        {
            set_data(str);
            return *this;
        }


        String& operator=(const String& other)
        {
            if (copy_small_string(other) == false) {
                set_data(other.view());
            }

            return *this;
        }

        String& operator=(String&& other)
        {
            if (move_small_string(other) == false) {
                clear();
                _big.ptr = std::exchange(other._big.ptr, nullptr);
                _big.size = std::exchange(other._big.size, 0);
            }
            return *this;
        }

        String operator+(const String& other)
        {
            auto lhs_size = size();
            if (!lhs_size)
                return other;

            auto rhs_size = other.size();

            if (!rhs_size)
                return *this;

            //If either is 0, just return one or the other.
            String buffer{};
            buffer.create_data(lhs_size + rhs_size);

            //buffer._ptr
            char* dest_ptr = buffer.ptr();

            std::strncpy(dest_ptr, ptr(), lhs_size);
            dest_ptr += lhs_size;
            std::strncpy(dest_ptr, other.ptr(), rhs_size);

            return buffer;
        }

        String& operator+=(const String& other)
        {
            //I'd like to do this better
            return *this = operator+(other);
        }



        std::string string() const
        {
            return { ptr(), size() };
        }

        std::string_view view() const
        {
            return { ptr(), size() };
        }


        operator std::string() const
        {
            return string();
        }

        operator std::string_view() const
        {
            return view();
        }

    private:


        constexpr void clear() noexcept
        {
            if (!is_small() && _big.ptr)
                delete[] _big.ptr;

            _small = SmallData{};
        }

        bool copy_small_string(const String& other)
        {
            bool result = other.is_small();

            if (result) {
                std::memcpy(this, &other, sizeof(String));
            }

            return result;
        }

        bool move_small_string(String& other)
        {
            bool result = copy_small_string(other);

            if (result) {
                other.clear();
            }

            return result;
        }

        bool move_small_string(String&& other)
        {
            return move_small_string(other);
        }
        

        constexpr void create_data(size_t length) noexcept
        {
            clear();

            if (std::is_constant_evaluated() || is_length_small(length) == true) {
                _small.sizeByte = sign_value<int8_t> | static_cast<int8_t>(length);
            }
            else {
                _big.ptr = new char[length + 1];
                _big.ptr[length] = '\0';
                _big.size = length;
            }
        }


        constexpr void set_data(const std::string_view& str) noexcept
        {
            //If ownership becomes a worry, lexicon will be the thing that creates and manages these strings.

            if (str.empty() == true) {
                clear();
                return;
            }

            size_t length = str.size();

            create_data(length);


            std::char_traits<char>::copy(ptr(), str.data(), length);
            //std::strncpy(ptr(), str.data(), length);
        }
        

        [[nodiscard]] constexpr const char* ptr() const noexcept {
            return unconst(this)->ptr();
        }


        [[nodiscard]] constexpr char* ptr() noexcept {
            if (is_small() == true)
                return _small.chars;
            else
                return _big.ptr;
        }




        constexpr bool is_small() const noexcept
        {
            //The only way a lex string exist at const eval is if it's small.
            if (std::is_constant_evaluated() == false) {
                //If it's not constant evaluated, regardless if it's big or small, this checks for both being zero,
                // as well as both being negative.
                return _big.size <= 0;
            }

            return true;
        }






        //I could get away with using a variant for this.


    public:

        [[nodiscard]] inline const char* data() const noexcept { return ptr(); }
        [[nodiscard]] inline const char* c_str() const noexcept { return data(); }
        constexpr size_t size() const noexcept {
            if (is_small() == true) {
                return _small.sizeByte & ~sign_value<int8_t>;
            }
            else {
                return _big.size;
            }
        }

        constexpr bool empty() const noexcept
        {
            return !size();
        }

        
        constexpr std::strong_ordering operator<=>(const String& other) const noexcept
        {
            auto len = size();

            if (auto size = other.size(); len != size) {
                if (len < size)
                    return std::strong_ordering::less;
                else
                    return std::strong_ordering::greater;
            }
            else if (len) {
                int test = std::char_traits<char>::compare(ptr(), other.ptr(), len + 1);

                if (test > 0)
                    return std::strong_ordering::greater;
                
                if (test < 0)
                    return std::strong_ordering::less;
            }

            return std::strong_ordering::equal;


            //return {};
            //*
            if (std::is_constant_evaluated()) {
                const char* lhs = ptr();
                const char* rhs = other.ptr();

                std::strcmp(lhs, rhs);

                auto length = std::min(size(), other.size());
                for (size_t i = 0; i < length; ++i) {
                    if (auto result = lhs[i] <=> rhs[i]; result != std::strong_ordering::equal)
                        return result;
                }

                return std::strong_ordering::equal;
            }
            else {
                int test = std::strncmp(ptr(), other.ptr(), std::min(size(), other.size()));

                if (test > 0)
                    return std::strong_ordering::greater;
                if (test < 0)
                    return std::strong_ordering::less;
                else
                    return std::strong_ordering::equal;

            }
            //*/
        }

        constexpr bool operator==(const String& other) const
        {
            return operator<=>(other) == std::strong_ordering::equal;
        }

        


    public:

        constexpr bool IsValueZero() const noexcept
        {
            return empty();
        }

        static TypeInfo* GetVariableType(const String*)
        {
            ITypeInfo* policy = IdentityManager::instance->GetTypeByOffset("STRING", 0);

            //Should already be specialized, so just sending it.
            return NULL_OP(NULL_Q(policy)->GetTypeInfo(nullptr));
        }
    private:
        /*

        union
        {
            char* _ptr = nullptr;
            char    _chars[1];//...

        };
        union
        {
            struct
            {
                uint8_t l_charBytes[7];
                int8_t l_sizeByte;
            };

            //struct
            //{
            //    int8_t lil_controlByte;
            //    uint8_t lil_testBytes[7];
            //};

            intptr_t _size = 0;
        };

        //*/

        /*

        union
        {
            char* _ptr = nullptr;
            char    _chars[1];//...

        };
        union
        {
            struct
            {
                uint8_t l_charBytes[7];
                int8_t l_sizeByte;
            };

            //struct
            //{
            //    int8_t lil_controlByte;
            //    uint8_t lil_testBytes[7];
            //};

            intptr_t _size = 0;
        };

        //*/

        union
        {
            
            SmallData   _small{};
            BigData     _big;
        };

        //*/
    };
    REQUIRED_SIZE(String, 0x10);


    //Make this any kind of string type.
	template <>
	struct VariableType<std::string>
	{

		TypeInfo* operator()()
		{
			return String::GetVariableType(nullptr);
		}
	};

	template <>
	struct VariableType<std::string_view>
	{

		TypeInfo* operator()()
		{
			return String::GetVariableType(nullptr);
		}
	};




}

#ifndef __INTELLISENSE__
//Turning off intellisense on this because it's annoying as shit.
template <>
struct fmt::formatter<LEX::String>
{
    template <class ParseContext>
    constexpr auto parse(ParseContext& a_ctx)
    {
        return a_ctx.begin();
    }


    // Forwards the formatting by casting the enum to it's underlying type
    auto format(const LEX::String& str, fmt::format_context& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", str.view());;
    }
};
#endif
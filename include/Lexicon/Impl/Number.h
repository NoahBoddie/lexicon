#pragma once

//src
#include "TypeID.h"


template <typename EnumType>
requires(std::is_enum_v<EnumType>&& !std::is_scoped_enum_v<EnumType>) struct fmt::formatter<EnumType> : fmt::formatter<std::underlying_type_t<EnumType>>
{
	// Forwards the formatting by casting the enum to it's underlying type
	auto format(const EnumType& enumValue, format_context& ctx) const
	{
		return fmt::formatter<std::underlying_type_t<EnumType>>::format(
			static_cast<std::underlying_type_t<EnumType>>(enumValue), ctx);
	}
};





namespace LEX
{
    
#define BINARY_OPERATOR(mc_symbol, ...)                                                                     \
        operator mc_symbol(const Number& a_rhs) {                                                           \
            constexpr bool assign = _IsAssign(STRINGIZE(mc_symbol));                                        \
            if constexpr (!assign) {                                                                        \
                if ((!IsValid() && strcmp(STRINGIZE(mc_symbol), "=") != 0) || a_rhs.IsValid() == false) {   \
                    report::runtime::fatal("some number is invalid.");                                      \
                    throw nullptr;                                                                          \
                }                                                                                           \
            }                                                                                               \
            Number result = std::visit([&](auto&& lhs) -> Number {                                          \
                return std::visit([&](auto&& rhs) -> Number {                                               \
                    if constexpr (true __VA_OPT__(&&) __VA_ARGS__) {                                        \
                        return lhs mc_symbol rhs;                                                           \
                    }                                                                                       \
                    else {                                                                                  \
                        report::runtime::fatal("invalid ops used on each other.");                          \
                        throw nullptr;                                                                      \
                        return lhs;                                                                         \
                    }                                                                                       \
                    }, a_rhs._value);                                                                       \
                }, _value);                                                                                 \
            if constexpr (assign)                                                                           \
                return *this;                                                                               \
            else                                                                                            \
                return result;                                                                              \
        }

    //Needs work, as this can assign with ++
#define UNARY_OPERATOR(mc_symbol, ...)                                                                      \
        operator mc_symbol() {                                                                              \
            constexpr bool assign = _IsAssign(STRINGIZE(mc_symbol));                                        \
            if constexpr (!assign) {                                                                        \
                if (!IsValid() && strcmp(STRINGIZE(mc_symbol), "=") != 0) {                                 \
                    report::runtime::fatal("some number is invalid.");                                      \
                    throw nullptr;                                                                          \
                }                                                                                           \
            }                                                                                               \
            Number result = std::visit([&](auto&& lhs) -> Number {                                          \
                 if constexpr (true __VA_OPT__(&&) __VA_ARGS__) {                                           \
                        return mc_symbol lhs;                                                               \
                    }                                                                                       \
                    else {                                                                                  \
                        report::runtime::fatal("invalid ops used on number.");                              \
                        throw nullptr;                                                                      \
                        return lhs;                                                                         \
                    }                                                                                       \
                }, _value);                                                                                 \
            if constexpr (assign)                                                                           \
                return *this;                                                                               \
            else                                                                                            \
                return result;                                                                              \
        }

    //Visit in the above operator doesn't work.
    //#define BINARY_OPERATOR(mc_symbol, ...) static CONCAT(test, __LINE__); static_assert(true);

    template<typename T>
    concept numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

    //Determines how the number reacts to reaching its limits. Whether it hits infinity or overflows and wraps around. 
    // Integers are naturally aligned to overflow, floats don't wrap
	ENUM(Limit, uint8_t)
    {
        //Limit, the smallest value must always have a value
        //I'm thinking about getting rid of limit. Overflow for integers, infinity for floats.
        Overflow = 1,
        Infinite,
        Total,

        Shift = 0,//uses 2 bytes
        Invalid = 0xFF
    };

    //enum that determines the final size of the number.
    ENUM(Size, uint8_t)
    {
        //Size 
        Bit,       //Represents a single bit. This value is used for boolean, and negates all other values
        //Byte and word sizes are useless for this.
        Byte,           //A single byte to represent its value
        Word,
        DWord,
        QWord,
        Total,
        Shift = Limit::Shift + 2,//Uses 3 bytes
        //Total bytes, 3
        Invalid = 0xFF
    };

    //Determines if the value is percieved as signed or not.
    ENUM(Signage, uint8_t)
    {
        Signed,
        Unsigned,
        Total,

        Shift = Size::Shift + 3,//Uses 1 byte
        Invalid = 0xFF
    };
    
    //The actual way it accounts for data. Integral uses integers while floating uses floating point values like doubles and floats. As on the tin so it is.
    ENUM(NumeralType, uint8_t)
    {
        Integral,
        Floating,
        Total,
        Shift = Signage::Shift + 1,//Uses 1 bytes
        Invalid = 0xFF
    };

    //2 + 3 + 1 + 1
    //Total bytes, 7

    constexpr static uint8_t GetOffset_(NumeralType type, Size size, Signage sign, Limit limit)
    {
        uint8_t result = 0;
        //Old entries marked right
        result |= type << NumeralType::Shift;   //24
        result |= size << Size::Shift;          //16
        result |= sign << Signage::Shift;       //8
        result |= limit << Limit::Shift;        //0

        return result;
    }


    enum struct direction
    {

        left,
        right
    };
    //not using, unsure if I want
    template <std::integral T>
    T _UnsetPastBit(T value, size_t shift, direction dir)
    {
        bool done = false;
        if (!done && dir == direction::left)
        {
        __left:
            value <<= shift;

        }

        if (!done && dir == direction::right)
        {
        __right:
            value >>= shift;

        }

        if (!done)
        {
            done = true;
            if (dir == direction::left)         goto __right;
            else if (dir == direction::right)   goto __left;
        }

        return value;
    }

    //TODO: Make number smaller by packing the settings into the variant.
    struct Number
    {


        struct Settings
        {
            


            Settings() = default;

            template <numeric T>
            Settings()
            {
                *this = Settings::CreateFromType<T>();
            }

            //Might not use this.
            Settings(NumeralType tp, Size sz, Signage sg, Limit lm) :
                type{ tp }, size{ sz }, sign{ sg }, limit{ lm }
            {

            }



            //It's 7 bits long. Want to use GetOffset but it's 
            static constexpr TypeOffset length = 0x7F;//GetOffset(NumeralType::Floating, Size::QWord, Signage::Unsigned, Limit::Infinite);//


            //Unionize this for easy reading
            NumeralType type{ NumeralType::Invalid };
            Size        size{ Size::Invalid };
            Signage     sign{ Signage::Invalid };
            Limit       limit{ Limit::Invalid };


            constexpr uint8_t GetOffset() const
            {
                uint8_t result = 0;
                //Old entries marked right
                result |= type << NumeralType::Shift;   //24
                result |= size << Size::Shift;          //16
                result |= sign << Signage::Shift;       //8
                result |= limit << Limit::Shift;        //0

                return result;
            }

            constexpr static uint8_t GetOffset(NumeralType type, Size size, Signage sign, Limit limit)
            {
                uint8_t result = 0;
                //Old entries marked right
                result |= type << NumeralType::Shift;   //24
                result |= size << Size::Shift;          //16
                result |= sign << Signage::Shift;       //8
                result |= limit << Limit::Shift;        //0

                return result;
            }

            constexpr static uint8_t GetOffset(NumeralType type)
            {
                //To get offset like this Might instead use a type.
                return GetOffset(type, Size::QWord, Signage::Signed, type == NumeralType::Floating ? Limit::Infinite : Limit::Overflow);
            }

            
            static Settings CreateFromID(TypeID id)
            {
                //This could be constexpr, though no point innit?

                //make create from offset.
                if (id != 0 && id <= length)
                {
                    uint8_t value = static_cast<uint8_t>(id);
                    uint8_t temp = static_cast<uint8_t>(id);

                    Settings result{};

                    //messy but I hope it works.

                    temp = value >> NumeralType::Shift;
                    result.type = static_cast<NumeralType>(temp);
                    value &= ~(temp << NumeralType::Shift);

                    temp = value >> Size::Shift;
                    result.size = static_cast<Size>(temp);
                    value &= ~(temp << Size::Shift);

                    temp = value >> Signage::Shift;
                    result.sign = static_cast<Signage>(temp);
                    value &= ~(temp << Signage::Shift);

                    //temp = value >> Limit::Shift;//No need.
                    result.limit = static_cast<Limit>(temp);
                    //value &= ~temp;//Also no need.

                    return result;

                }
                else
                {
                    return Settings{};
                }
            }


            constexpr std::strong_ordering operator <=> (const Settings& rhs) const = default;
            
            /*
            {
                //The conversion rules follow the same rules as the code range for it's type, that it is the combined value (via OR) of all its settings.
                // If one is a float and the other isn't it becomes the floating one.
                // If one is larger than the other in size it becomes the larger one.
                // If it is unsigned and the other isn't it becomes the unsigned one.
                // Lastly, there might be something about the looping types.


                auto left = GetOffset();
                auto right = rhs.GetOffset();

                return left <=> right;
            }
            //*/


            bool IsFloat() const { return type == NumeralType::Floating; }
            bool IsInteger() const { return type == NumeralType::Integral; }

            bool IsUnsigned() const { return sign == Signage::Unsigned; }
            bool IsSigned() const { return sign == Signage::Signed; }


            double MaxF()
            {
                constexpr int64_t t = 1;
                constexpr uint32_t s = 1;

                constexpr auto r = t + s;

                return 0;
            }
            double MinF()
            {
                return 0;
            }


            double MaxI()
            {
                return 0;
            }
            double MinI()
            {
                return 0;
            }
            

            template <numeric T>
            constexpr static Settings CreateFromType()
            {
                Settings setting{};

                //Type & Limit
                if constexpr (std::is_floating_point_v<T>) {
                    setting.type = NumeralType::Floating;
                    setting.limit = Limit::Infinite;
                }
                else {
                    setting.type = NumeralType::Integral;
                    setting.limit = Limit::Overflow;
                }

                //Size
                if constexpr (!std::is_same_v<T, bool>) {
                    switch (sizeof(T))
                    {
                        //Literally impossible but hey, never know in the future.
                    default: return Settings{};
                    case 8: setting.size = Size::QWord; break;
                    case 4: setting.size = Size::DWord; break;
                    case 2: setting.size = Size::Word; break;
                    case 1: setting.size = Size::Byte; break;

                    }
                }
                else {
                    setting.size = Size::Bit;
                    //Might have bools specifically lock for this sort of thing.
                    setting.limit = Limit::Infinite;
                }

                //Signage
                if constexpr (std::is_unsigned_v<T>) {
                    setting.sign = Signage::Unsigned;
                }
                else {
                    setting.sign = Signage::Signed;
                }

                return setting;
            }


        };


        Number() = default;
        
        //*
        template <numeric T>// requires (!std::is_integral_v<T>)
        Number(T v) :
           // _value{ v },
            _setting{ Settings::CreateFromType<T>() },
            _priority{ _setting.GetOffset() }
        {
            //This is the list of concessions made to make sure the variant works.
            //Also thinking of making void the first entry
            if constexpr (std::is_integral_v<T>)
            {
                if constexpr (std::is_signed_v<T>){
                    _value.emplace<int64_t>(v);
                    //_value = static_cast<int64_t>(v);
                }
                else {
                    _value.emplace<uint64_t>(v);
                    //_value = static_cast<uint64_t>(v);
                }
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                _value.emplace<double>(v);
                //_value = static_cast<double>(v);
            }
            else 
            {
                _value.emplace<T>(v);
                //_value = v;
            }

            RGL_LOG(trace, "Number<{}> ctor {}, sets: sign {} size {} num {} lim {}", typeid(T).name(),
                static_cast<std::string>(*this),
                _setting.sign, _setting.size, _setting.type, _setting.limit);
        }

        Number(Settings _set) :
            // _value{ v },
            _setting{ _set },
            _priority{ _setting.GetOffset() }
        {
            const char* name = "INVALID";

            if (_setting.IsFloat() == true)
            {
                _value.emplace<double>(0);
                name = typeid(double).name();
            }
            else if (_setting.IsInteger() == true)
            {
                if (_setting.IsSigned() == true)
                {
                    _value.emplace<int64_t>(0);
                    name = typeid(int64_t).name();
                }
                else if (_setting.IsUnsigned() == true)
                {
                    _value.emplace<uint64_t>(0);
                    name = typeid(uint64_t).name();
                }
            }


            RGL_LOG(trace, "Number<{}> ctor {}, sets: sign {} size {} num {} lim {}", name,
                static_cast<std::string>(*this),
                _setting.sign, _setting.size, _setting.type, _setting.limit);
        }
        //*/


        //Number& operator=(Number a_rhs) noexcept
        //{
        //    _value = a_rhs._value;
        //    return *this;
        //}


        constexpr std::partial_ordering operator <=> (Number rhs) const
        {
            return _value <=> rhs._value;
        }


        constexpr bool operator==(const Number& a_rhs) const
        {
            return operator<=>(a_rhs) == std::strong_ordering::equal;
        }


        int Cmp(const Number& a_rhs)
        {
            return _priority - a_rhs._priority;
        }





        //Move this.
        static constexpr bool _IsAssign(const std::string_view str)
        {
            if (str == "==" || str == "===" || str == "!=" || str == "!==")
                return false;

            return str.find('=') != std::string_view::npos;
        }

        bool IsValid() const
        {
            logger::trace("index in is valid {}", _value.index());

            return _value.index() != 0;

        }


        Number operator *(const Number& a_rhs) {
            constexpr bool assign = _IsAssign(STRINGIZE(*)); if constexpr (!assign) {
                if ((!IsValid() && strcmp(STRINGIZE(*), "=") != 0) || a_rhs.IsValid() == false) {
                    report::runtime::fatal("some number is invalid. left {} right {}", IsValid(), a_rhs.IsValid()); throw nullptr;
                }
            } Number result = std::visit([&](auto&& lhs) -> Number { return std::visit([&](auto&& rhs) -> Number { if constexpr (true) {
                return lhs * rhs;
            }
            else {
                report::runtime::fatal("invalid ops used on each other."); throw nullptr; return lhs;
            } }, a_rhs._value); }, _value); if constexpr (assign) return *this; else return result;
        };
        Number operator +(const Number& a_rhs) {
            RGL_LOG(info, "tell");
            constexpr bool assign = _IsAssign(STRINGIZE(+)); if constexpr (!assign) {
                if ((!IsValid() && strcmp(STRINGIZE(+), "=") != 0) || a_rhs.IsValid() == false) {
                    report::runtime::fatal("some number is invalid. left: {} right: {}", IsValid(), a_rhs.IsValid()); throw nullptr;
                }
            } Number result = std::visit([&](auto&& lhs) -> Number { return std::visit([&](auto&& rhs) -> Number { if constexpr (true) {
                return lhs + rhs;
            }
            else {
                report::runtime::fatal("invalid ops used on each other."); throw nullptr; return lhs;
            } }, a_rhs._value); }, _value); if constexpr (assign) return *this; else return result;
        };
        Number BINARY_OPERATOR(/);
        Number BINARY_OPERATOR(-);
        Number BINARY_OPERATOR(%, std::is_integral_v<decltype(lhs)>&& std::is_integral_v<decltype(rhs)>);
        Number UNARY_OPERATOR(-);
        //Number UNARY_OPERATOR(!);
        
        //This set up allows us to be able to visit, but cause error on operators that can't actually use the value.
        //std::is_integral_v<decltype(lhs)> && std::is_integral_v<decltype(rhs)>


        explicit operator std::string() const
        {
            std::string result = "UNVISITED";

            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, char>)
                    result = "INVALID_NUMBER";
                else
                    //Should control its output based on below settings.
                    result = std::to_string(arg);

                }, _value);

            return result;
        }

        TypeOffset GetOffset()
        {
            //probably should just do the calculation but eh
            return _priority;
        }

        //If it's index is the first, it's value will be null and unassignable. Thus, this easiest signals that it's value is zero.
        std::variant<char, uint64_t, int64_t, double> _value;


        template<numeric T>
        bool As(T& out) const
        {
            constexpr auto query = Settings::CreateFromType<T>();
            //Make this a bit less exact in the future.
            
            if (query != _setting) {
                return false;
            }

            switch (_value.index())
            {
            case 1: 
                if constexpr (std::is_integral_v<T>)
                    out = _UInt(); 
                break;
            case 2: 
                if constexpr (std::is_integral_v<T>)
                    out = _SInt(); 
                break;
            case 3: 
                if constexpr (std::is_floating_point_v<T>)
                    out = _Float(); 
                break;

            default:
                return false;
            }
            return true;
        }

        
        double _Float() const
        {
            return std::get<double>(_value);
        }

        
        uint64_t _UInt() const
        {
            return std::get<uint64_t>(_value);
        }

        
        int64_t _SInt() const
        {
            return std::get<int64_t>(_value);
        }



        //mutable const?
        Settings _setting{};


        uint8_t _priority{};//helps determine which number should become the new number.
        //extra 3 bytes

        //If its being set to or from an unset type, it will handle the conversion





        //For now the only thing I'm doing is additive.

        //CreateNumber of size, limit, and signage(?) should possibly be a thing I need


        /*
        Type Priority, determines what type turns into another type when

        If an entry contains a floating point type, both will be compared as floating points.

        After that, there's the handling of 1 is unsigned and the other isn't, if both are signed, or if they are both unsigned.

        Whichever properties are taken are based on which is the left hand side,or the dominant among the operands (see floats).

        Actually, scratch. Dominant type only

        The priority is

        Floating point//4
        Larger type 2
        unsigned 1


        So unsigned double + signed char == unsigned double.

        //*/
    };
    static_assert(sizeof(Number) == 0x18);




    /*
    #include <iostream>
    #include <limits>
    #include <cmath>
    using namespace std;

    int main()
    {
        uint64_t max = 128;
        uint64_t test;
        //test = std::numeric_limits<uint64_t>::max();
        test = 255;
        //test = 256;//Should be 0

        //the result of these should not be the same, but for uint64's limit, they are
        // when shortened to uint8. It shouldn't be.
        //Correction. These should be the same. HOWEVER, it should be favoring -1.
        // * 2 on max size should double back, not set it to zero.
        //test *= 2;
        //test -= 1;
        float raw  = test / (float)max;
        auto loops  = test / max;
        float percent = raw - loops;
        auto remain = test % max;
        //The minus 1 is almost correct, but note it has to pass zero a buch of times.
        // actually, twas all wrong.

        //This is the prefered version because it works better with floats. Presumably.
        auto should = test - (max * (loops));
        cout<<"Test << " << test << "\n";
        cout<<"vals << " << raw << ", "  << (raw == loops) << ", " << loops << ", and " << remain << "\n";
        cout<<"Should << " << should;
        return 0;
    }
    //*/


    

}

namespace fmt
{
    /*
    
    template <typename Char>
	struct formatter<LEX::Signage, Char>
	{
		template <class ParseContext>
		constexpr auto parse(ParseContext& a_ctx)
		{
			return a_ctx.begin();
		}

		template <class FormatContext>
		auto format(const auto& num, FormatContext& a_ctx) const
		{
			return fmt::format_to(a_ctx.out(), "{}", (uint8_t)num);
		}
	};

    template <typename Char>
	struct formatter<LEX::NumeralType, Char>
	{
		template <class ParseContext>
		constexpr auto parse(ParseContext& a_ctx)
		{
			return a_ctx.begin();
		}

		template <class FormatContext>
		auto format(const auto& num, FormatContext& a_ctx) const
		{
			return fmt::format_to(a_ctx.out(), "{}", (uint8_t)num);
		}
	};

    template <typename Char>
	struct formatter<LEX::Size, Char>
	{
		template <class ParseContext>
		constexpr auto parse(ParseContext& a_ctx)
		{
			return a_ctx.begin();
		}

		template <class FormatContext>
		auto format(const auto& num, FormatContext& a_ctx) const
		{
			return fmt::format_to(a_ctx.out(), "{}", (uint8_t)num);
		}
	};

    
template <typename Char>
struct formatter<LEX::Limit, Char>
{ uint64_t t;
	template <class ParseContext>
	constexpr auto parse(ParseContext& a_ctx)
	{
		return a_ctx.begin();
	}

	template <class FormatContext>
	auto format(const LEX::Limit& num, FormatContext& a_ctx) const
	{
		return fmt::format_to(a_ctx.out(), "{}", (uint8_t)num);
	}
};


//*/
    /*
    template <typename Char>
    struct formatter<LEX::Number, Char>
    {
	    template <class ParseContext>
	    constexpr auto parse(ParseContext& a_ctx)
	    {
		    return a_ctx.begin();
	    }

	    template <class FormatContext>
	    auto format(const LEX::Number& num, FormatContext& a_ctx) const
	    {
		    return fmt::format_to(a_ctx.out(), "{}", static_cast<std::string>(num));
	    }
    };
    //*/
	template <>
    struct fmt::formatter<LEX::Number>
	{
		template <class ParseContext>
		constexpr auto parse(ParseContext& a_ctx)
		{
			return a_ctx.begin();
		}

        
		// Forwards the formatting by casting the enum to it's underlying type
		auto format(const LEX::Number& num, format_context& ctx) const
		{
			return fmt::format_to(ctx.out(), "{}", static_cast<std::string>(num));;
		}
	};
}



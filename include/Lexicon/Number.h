#pragma once

#include "VariableType.h"

//src
#include "Lexicon/TypeID.h"
#include "Lexicon/AbstractTypePolicy.h"
#include "Lexicon/Interfaces/IdentityManager.h"

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
    struct AbstractTypePolicy;


    //*
#define BINARY_OPERATOR(mc_symbol, ...)                                                                         \
        operator mc_symbol(const Number& other)                                                                 \
        {                                                                                                       \
            Number result = Operator(other,                                                                     \
                [](auto lhs, auto rhs)                                                                          \
                {                                                                                               \
                    if constexpr (true __VA_OPT__(&&) __VA_ARGS__) {                                            \
                        return lhs mc_symbol rhs;                                                               \
                    }                                                                                           \
                    else {                                                                                      \
                        report::runtime::break_critical("invalid ops used on each other.");                     \
                        return lhs;/*This just helps the thing know it definitely has a return type.*/          \
                    }                                                                                           \
                });                                                                                             \
            return result;                                                                                      \
        }



    //Needs work, as this can assign with ++
#define PRE_UNARY_OPERATOR(mc_symbol, ...)                                                                  \
        operator mc_symbol()                                                                                \
        {                                                                                                   \
            Number result = Operator([](auto self)                                                          \
                {                                                                                           \
                    if constexpr (true __VA_OPT__(&&) __VA_ARGS__)                                          \
                    {                                                                                       \
                        return mc_symbol self;                                                              \
                    }                                                                                       \
                    else {                                                                                  \
                        report::runtime::break_critical("invalid ops used on each other.");                 \
                        return self;                                                                        \
                    }                                                                                       \
                });                                                                                         \
            return result;                                                                                  \
        }
        


    
    //Visit in the above operator doesn't work.
    //#define BINARY_OPERATOR(mc_symbol, ...) static CONCAT(test, __LINE__); static_assert(true);

    //Remove use of shift in all of these.

    //Determines how the number reacts to reaching its limits. Whether it hits infinity or overflows and wraps around. 
    // Integers are naturally aligned to overflow, floats don't wrap
	ENUM(Limit, uint8_t)
    {
        //Limit, the smallest value must always have a value
        //I'm thinking about getting rid of limit. Overflow for integers, infinity for floats.
        Overflow,
        Bound,
        Infinite,
        Total,
        Last = Limit::Total - 1,

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
        Last = Size::Total - 1,

        Invalid = 0xFF
    };

    //Determines if the value is percieved as signed or not.
    ENUM(Signage, uint8_t)
    {
        Signed,
        Unsigned,
        Total,
        Last = Signage::Total - 1,

        Invalid = 0xFF,
    };
    
    //The actual way it accounts for data. Integral uses integers while floating uses floating point values like doubles and floats. As on the tin so it is.
    ENUM(NumeralType, uint8_t)
    {
        Integral,
        Floating,
        Total,
        Last = NumeralType::Total - 1,
        
        Invalid = 0xFF,
    };

    //2 + 3 + 1 + 1
    //Total bytes, 7
    
    constexpr uint8_t limit_load = 0;//there is no limit load.
    constexpr uint8_t sizeOffsetLoad = Limit::Total;
    constexpr uint8_t signOffsetLoad = sizeOffsetLoad * Size::Total;
    constexpr uint8_t typeOffsetLoad = signOffsetLoad * Signage::Total;


    constexpr uint8_t GetNumberOffset(NumeralType type, Size size, Signage sign, Limit limit)
    {
        
        if (type == NumeralType::Invalid ||
            size == Size::Invalid ||
            sign == Signage::Invalid ||
            limit == Limit::Invalid) {
            return 0;
        }
            

        return typeOffsetLoad * type + signOffsetLoad * sign + sizeOffsetLoad * size + limit + 1;
    }
    
    


    enum struct NumberDataType
    {
        Invalid,
        Float,
        SInt,
        UInt,
    };


    union NumberData
    {

        size_t									_raw = 0;
        std::array<uint8_t, sizeof(size_t)>		_bytes;
        double									floating;
        int64_t									sInteger;
        uint64_t								uInteger;

        template<numeric T>
        auto& Get()
        {
            if constexpr (std::is_floating_point_v<T>) {
                return floating;
            }
            else if constexpr (std::is_same_v<T, uint64_t>) {
                return uInteger;
            }
            else {
                return sInteger;
            }
        }



        constexpr NumberData() = default;
        constexpr NumberData(double arg) : floating{ arg } {}//doesn't matter which it is.
        constexpr NumberData(int64_t arg) : sInteger{ arg } {}//doesn't matter which it is.
        constexpr NumberData(uint64_t arg) : uInteger{ arg } {}//doesn't matter which it is.
        constexpr NumberData(int32_t arg) : sInteger{ arg } {}
        constexpr NumberData(uint32_t arg) : sInteger{ arg } {}
        constexpr NumberData(int16_t arg) : sInteger{ arg } {}
        constexpr NumberData(uint16_t arg) : sInteger{ arg } {}
        constexpr NumberData(int8_t arg) : sInteger{ arg } {}
        constexpr NumberData(uint8_t arg) : sInteger{ arg } {}
        constexpr NumberData(bool arg) : sInteger{ arg } {}
    };
    REQUIRED_SIZE(NumberData, 0x8);


    struct NumberLimit
    {
        NumberData min;
        NumberData max;
        NumberData posInf;
        NumberData negInf;

        constexpr NumberLimit() = default;

        constexpr NumberLimit(NumberData _min, NumberData _max, std::optional<NumberData> pos_inf = std::nullopt, std::optional<NumberData> neg_inf = std::nullopt) :
            min{ _min }, max{ _max }, negInf{ neg_inf.value_or(0) }, posInf{ pos_inf.value_or(0) }
        {

        }
    };

    //A few things, I want the access of this to be a function, second, this function should start an initializer.
    // This would allow me to make
    //Note, this isn't going to be accessed from one place, probably should be allowed to migrate be based on whatever system it's on.
    inline NumberLimit limitMap[NumeralType::Total][Signage::Total][Size::Total];


    inline void InitLimitMap()
    {
        static bool init = false;

        if (init)
            return;

        init = true;

        //For float, bit is episilon, byte is percent word. Nothing is used for word.


        //Note, these are given infinity REGARDLESS if it actually can use infinity or not
        // Also note, the value of infinity is a constant. Maybe also the only time that it will not be cleared.
        // An infinity of 0 means no infinity at all.
        limitMap[NumeralType::Floating][Signage::Signed][Size::Bit] = { -1.0, 1.0, INFINITY, -INFINITY };
        limitMap[NumeralType::Floating][Signage::Signed][Size::Byte] = { -100.0, 100.0, INFINITY, -INFINITY };
        limitMap[NumeralType::Floating][Signage::Signed][Size::Word] = { -100.0, 100.0, INFINITY, -INFINITY };
        limitMap[NumeralType::Floating][Signage::Signed][Size::DWord] = { min_value<float>, max_value<float>, INFINITY, -INFINITY };
        limitMap[NumeralType::Floating][Signage::Signed][Size::QWord] = { min_value<double>, max_value<double>, INFINITY, -INFINITY };

        limitMap[NumeralType::Floating][Signage::Unsigned][Size::Bit] = { 0.0, 1.0, 0, INFINITY };
        limitMap[NumeralType::Floating][Signage::Unsigned][Size::Byte] = { 0.0, 100.0, 0, INFINITY };
        limitMap[NumeralType::Floating][Signage::Unsigned][Size::Word] = { 0.0, 100.0, 0, INFINITY };
        limitMap[NumeralType::Floating][Signage::Unsigned][Size::DWord] = { 0.0, max_value<float>, 0, INFINITY };
        limitMap[NumeralType::Floating][Signage::Unsigned][Size::QWord] = { 0.0, max_value<double>, 0, INFINITY };



        limitMap[NumeralType::Integral][Signage::Unsigned][Size::Bit] = { (int64_t)false, (int64_t)true };
        limitMap[NumeralType::Integral][Signage::Unsigned][Size::Byte] = { min_value<uint8_t>, max_value<uint8_t>, max_value<uint64_t> };
        limitMap[NumeralType::Integral][Signage::Unsigned][Size::Word] = { min_value<uint16_t>, max_value<uint16_t>, max_value<uint64_t> };
        limitMap[NumeralType::Integral][Signage::Unsigned][Size::DWord] = { min_value<uint32_t>, max_value<uint32_t>, max_value<uint64_t> };
        limitMap[NumeralType::Integral][Signage::Unsigned][Size::QWord] = { min_value<uint64_t>, max_value<uint64_t>, max_value<uint64_t> };

        limitMap[NumeralType::Integral][Signage::Signed][Size::Bit] = { -1, 1, max_value<int64_t>, min_value<int64_t> };
        limitMap[NumeralType::Integral][Signage::Signed][Size::Byte] = { min_value<int8_t>, max_value<int8_t>, max_value<int64_t>, min_value<int64_t> };
        limitMap[NumeralType::Integral][Signage::Signed][Size::Word] = { min_value<int16_t>, max_value<int16_t>, max_value<int64_t>, min_value<int64_t> };
        limitMap[NumeralType::Integral][Signage::Signed][Size::DWord] = { min_value<int32_t>, max_value<int32_t>, max_value<int64_t>, min_value<int64_t> };
        limitMap[NumeralType::Integral][Signage::Signed][Size::QWord] = { min_value<int64_t>, max_value<int64_t>, max_value<int64_t>, min_value<int64_t> };

    }


    inline NumberLimit GetNumberLimit(NumeralType type, Signage sign, Size size)
    {
        InitLimitMap();
        return limitMap[type][sign][size];
    }





    INITIALIZE()
    {
        //there are 3 limit types


        InitLimitMap();
    }


    ENUM(InfiniteState, int8_t)
    {
        Negative = -1,
        Finite = 0,
        Positive = 1,
    };


    struct infinity
    {

        //This is a struct to represent infinity for whatever current setting is active. Helps represent infinity for types that don't actually have it.

        bool positive = true;
        NumeralType type{ NumeralType::Invalid };
        Size        size{ Size::Invalid };
        Signage     sign{ Signage::Invalid };

        //Also a note to this, I'd like to make constexpr versions of infinity to represent each version of infinity.


        infinity operator-() const
        {
            infinity result = *this;
            if (sign != Signage::Unsigned)
                result.positive = !result.positive;
            else
                report::message::break_warn("infinity cannot be negated with current settings");

            return result;
        }


        constexpr infinity() = default;

        constexpr infinity(NumeralType nt, Size sz, Signage sg) : type{ nt }, size{ sz }, sign{ sg }
        {}


        //returns a constant for use when one wants to set a number to an infinite, but also doesn't want to 
        // override the settings.
        inline static constexpr infinity constant()
        {
            return infinity{};
        }

        inline static constexpr infinity float32()
        {
            return infinity{ NumeralType::Floating, Size::DWord, Signage::Signed };
        }

        inline static constexpr infinity float64()
        {
            return infinity{ NumeralType::Floating, Size::QWord, Signage::Signed };
        }

        inline static constexpr infinity ufloat32()
        {
            return infinity{ NumeralType::Floating, Size::DWord, Signage::Unsigned };
        }

        inline static constexpr infinity ufloat64()
        {
            return infinity{ NumeralType::Floating, Size::QWord, Signage::Unsigned };
        }




        //Also, positive will be restricted, instead, you must perform unary minus on infinity to get negative infinity.
    };



    struct NumberConstant
    {
        struct {} static inline Maybe;
    };



	struct Number
	{

        struct Settings
        {



            constexpr Settings() = default;

            template <numeric T>
            constexpr Settings()
            {
                *this = Settings::CreateFromType<T>();
            }

            //Might not use this.
            constexpr Settings(NumeralType tp, Size sz, Signage sg, Limit lm) :
                type{ tp }, size{ sz }, sign{ sg }, limit{ lm }
            {

            }



            //It's 7 bits long. Want to use GetOffset but it's 
            static constexpr TypeOffset length = GetNumberOffset(NumeralType::Last, Size::Last, Signage::Last, Limit::Last);


            //Unionize this for easy reading
            NumeralType type{ NumeralType::Invalid };
            Size        size{ Size::Invalid };
            Signage     sign{ Signage::Invalid };
            Limit       limit{ Limit::Invalid };


            constexpr uint8_t GetOffset() const
            {

                return GetNumberOffset(type, size, sign, limit);
            }

            constexpr static uint8_t GetOffset(NumeralType type, Size size, Signage sign, Limit limit)
            {
                return GetNumberOffset(type, size, sign, limit);
            }

            constexpr static uint8_t GetOffset(NumeralType type)
            {
                //To get offset like this Might instead use a type.
                return GetOffset(type, Size::QWord, Signage::Signed, type == NumeralType::Floating ? Limit::Infinite : Limit::Overflow);
            }

            static Settings CreateFromOffset(TypeOffset offset)
            {
                //This could be constexpr, though no point innit?

                if (offset >= length)
                    return {};

                offset--;

                Settings result{};

                auto set = (offset / typeOffsetLoad);

                result.type = (NumeralType)set;
                offset -= typeOffsetLoad * set;

                set = (offset / signOffsetLoad);

                result.sign = (Signage)set;
                offset -= signOffsetLoad * set;


                set = (offset / sizeOffsetLoad);

                result.size = (Size)set;
                offset -= sizeOffsetLoad * set;


                //What remains is likely the limit.
                result.limit = (Limit)offset;


                return result;
            }


            static Settings CreateFromID(TypeID id)
            {
                //This could be constexpr, though no point innit?

                TypeIdentity identity = IdentityManager::instance->GetIdentityFromID(id);

                //make create from offset.
                if (auto offset = identity.offset; id != 0 && id <= offset)
                {
                    //This gets rid of the spot for number in the offset.
                    offset--;

                    Settings result{};

                    auto set = (offset / typeOffsetLoad);

                    result.type = (NumeralType)set;
                    offset -= typeOffsetLoad * set;

                    set = (offset / signOffsetLoad);

                    result.sign = (Signage)set;
                    offset -= signOffsetLoad * set;


                    set = (offset / sizeOffsetLoad);

                    result.size = (Size)set;
                    offset -= sizeOffsetLoad * set;


                    //What remains is likely the limit.
                    result.limit = (Limit)offset;


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

            bool IsBoolean() const { return type == NumeralType::Integral && size == Size::Bit; }

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
                    setting.limit = Limit::Bound;
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

        Number(decltype(NumberConstant::Maybe) m) :
            _setting{ NumeralType::Integral, Size::Bit, Signage::Signed, Limit::Bound }, 
            _data { GetNumberLimit(NumeralType::Integral, Signage::Signed, Size::Bit).min },
            _priority{ _setting.GetOffset() }
        {
        }

        Number BINARY_OPERATOR(+);
        Number BINARY_OPERATOR(/);
        Number BINARY_OPERATOR(*);
        Number BINARY_OPERATOR(-);
        Number BINARY_OPERATOR(%, std::is_integral_v<decltype(lhs)>&& std::is_integral_v<decltype(rhs)>);
        Number PRE_UNARY_OPERATOR(-);
        
        constexpr std::strong_ordering operator <=> (Number other) const
        {
            
            return Visit([&](auto lhs)
            {
                return other.Visit([&](auto rhs)
                {
                    if (lhs < rhs)
                        return std::strong_ordering::less;
                    else if (lhs > rhs)
                        return std::strong_ordering::greater;
                    else
                        return std::strong_ordering::equal;
                });
            });

        }


        constexpr bool operator==(const Number& other) const
        {
            return operator<=>(other) == std::strong_ordering::equal;
        }
            
        static AbstractTypePolicy* GetVariableType(const Number* it)
        {
            ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset("NUMBER", !it ? 0 : it->GetOffset());

            //Should already be specialized, so just sending it.
            return policy->FetchTypePolicy(nullptr);
        }


		//Proving line.


		constexpr bool IsInfinite() const
		{
			return infinite != InfiniteState::Finite;
		}

		NumberDataType GetNumberType() const
		{
			if (_setting.IsFloat() == true)
			{
				return NumberDataType::Float;
			}
			else if (_setting.IsInteger() == true)
			{
				if (_setting.IsSigned() == true)
				{
					return NumberDataType::SInt;
				}
				else if (_setting.IsUnsigned() == true)
				{
					return NumberDataType::UInt;
				}
			}

			return NumberDataType::Invalid;
		}

        TypeOffset GetOffset() const
        {
            //probably should just do the calculation but eh
            return _priority;
        }

		template <typename V>
		decltype(auto) Visit(V visitor)
		{
			//Visit will visit with a version of number data's different data types, or fail if the settings are invalid.

			switch (GetNumberType())
			{
			case NumberDataType::Float:
				return visitor(_data.floating);

			case NumberDataType::SInt:
				return visitor(_data.sInteger);

			case NumberDataType::UInt:
				return visitor(_data.uInteger);

			default:
				report::error("Invalid number settings. Data cannot be visited.");
				break;
			}
		}

		template <typename V>
		constexpr decltype(auto) Visit(V visitor) const
		{
			//Visit will visit with a version of number data's different data types, or fail if the settings are invalid.

			switch (GetNumberType())
			{
			case NumberDataType::Float:
				return visitor(_data.floating);

			case NumberDataType::SInt:
				return visitor(_data.sInteger);

			case NumberDataType::UInt:
				return visitor(_data.uInteger);

			default:
				report::error("Invalid number settings. Data cannot be visited.");
				break;
			}
		}




		Number Convert(Settings settings) const
		{
			//This is a really lazy and also incorrect way to do this.

			Number copy = settings;

			//copy._data = other._data;


			//The limit type observed on this is changed to Maximum(limits) if left is unsigned, and right is negative infinity.

			if (infinite && copy._setting.limit != Limit::Infinite)
			{
				auto _neg = GetNumberLimit(copy._setting.type, copy._setting.sign, copy._setting.size).negInf;
				auto _pos = GetNumberLimit(copy._setting.type, copy._setting.sign, copy._setting.size).posInf;



				switch (infinite)
				{
				case InfiniteState::Positive:
					copy._data = _pos;
					break;
				case InfiniteState::Negative:
					report::debug("neg");
					copy._data = _neg;
					break;
				}

				if (_setting.limit == Limit::Infinite) {
					copy.SetInfinite(infinite);
					return copy;
				}
			}

			auto& other = *this;

			copy.Visit([&]<typename L>(L & lhs)
			{
				other.Visit([&]<typename R>(R rhs)
				{
					lhs = (L)rhs;
				});

				copy.NewLimitCheck();
			});

			return copy;
		}

		Number& Assign(const Number& other)
		{
			*this = other.Convert(_setting);
			return *this;
		}


		std::string ToString() const
		{
			switch (infinite)
			{
			case InfiniteState::Finite:
                if (_setting.IsBoolean() == true)
                {
                    switch (Get<int>())
                    {
                    case -1:
                        return "maybe";
                    case  0:
                        return "false";
                    case  1:
                        return "true";

                    default:
                        report::warn("invalid boolean value detected");
                        return "inv bool";
                    }
                    
                }
                else
                {
                    return Visit([](auto it) { return std::to_string(it); });
                }
			case InfiniteState::Positive:
				return "inf";
			case InfiniteState::Negative:
				return "-inf";
			}


			return "INVALID";
		}



		//I'd like to change the name but this is good.
        //This really should just take a setting and run through it. It using a number is a detriment
		Settings CompareSettings(const Number& other, Settings ret)
		{
			constexpr Settings k_int{ NumeralType::Integral, Size::DWord, Signage::Signed, Limit::Overflow };
			constexpr auto k_offset = k_int.GetOffset();


			Settings result = _setting;

			if (auto settings = other._setting; result.GetOffset() < settings.GetOffset())
				result = settings;

			if (result.GetOffset() < k_offset)
				result = k_int;


			if (result.GetOffset() < ret.GetOffset())
				result = ret;

			return result;
			//other is the right hand side, "this" is the left hand side, and ret is the return of the predicate turned into a number setting.
			//The largest ont between these options, and int wins.
		}





		void SetInfinite(InfiniteState state)
		{
			//This is just so the same lambda isn't floating around.

			if (Visit([](auto it) { return it == 0; }) == false)
				infinite = state;
		}


		void NewLimitCheck(int overflow = 0)
		{
			auto _limit = GetNumberLimit(_setting.type, _setting.sign, _setting.size);

			auto _min = _limit.min;
			auto _max = _limit.max;

			if (!overflow)
			{
				overflow = Visit(
					[&]<typename T>(T value) -> int
				{
					return value > _max.Get<T>() ?
						1 : value < _min.Get<T>() ?
						-1 : 0;
				});


			}

			if (!overflow)
				return;


			switch (_setting.limit)
			{
			case Limit::Overflow:
			{
				Visit([&]<typename T>(T & value)
				{
					using Cast = std::conditional_t<std::is_floating_point_v<T>, double, size_t>;
					value -= (_setting.IsSigned() + (size_t)(value / ((size_t)_max.Get<T>() + 1))) * ((size_t)_max.Get<T>() + 1);
				});
			}
			break;

			case Limit::Bound:
			{
				Visit([&](auto& value)
					{
						if (overflow == 1)
							_data = _max;
						else if (overflow == -1)
							_data = _min;
					});
			}
			break;

			case Limit::Infinite:
			{
				InfiniteState state = InfiniteState::Finite;

				if (overflow == 1) {
					_data = _limit.posInf;
					state = InfiniteState::Positive;
				}
				else if (overflow == -1) {
					_data = _limit.negInf;
					state = InfiniteState::Negative;
				}

				SetInfinite(state);
			}
			break;

			default:
				report::message::error("Invalid limit.");
			}
		}

		template <typename Func>
		Number Operator(Func func)
		{
			using Result = std::invoke_result_t<Func, int8_t>;

            constexpr Settings bool_settings = Settings::CreateFromType<bool>();

            constexpr Settings res_settings = Settings::CreateFromType<Result>();


            Number result = bool_settings == res_settings ? res_settings : CompareSettings(*this, res_settings);


			constexpr bool no_limit_check = requires(int self)
			{
				func(self, 0);
			};

			double comp;


			result.Visit([&](auto& res)
				{
					this->Visit([&]<typename L>(L& self)
					{
                        res = func(self);

                        //This should also check if doubles can play.
                        if constexpr (!no_limit_check)
                            comp = func((double)self);
					});
				});



			//Should require an integral

			if constexpr (!no_limit_check)
			{
				int overflow = result.Visit([comp]<typename T>(T res) -> int
				{
					return  res > comp ?
						-1 : res < comp ?
						1 : 0;
				});


				//report::trace("overflow {}", overflow);
				//bool bound = false;

				result.NewLimitCheck(overflow);
			}

			return result;
		}


        template <typename Func>
        Number Operator(const Number& other, Func func)
        {
            using Result = std::invoke_result_t<Func, int8_t, int8_t>;

            constexpr Settings bool_settings = Settings::CreateFromType<bool>();

            constexpr Settings res_settings = Settings::CreateFromType<Result>();


            Number result = bool_settings == res_settings ? res_settings : CompareSettings(other, res_settings);


            constexpr bool no_limit_check = requires(int lhs, int rhs)
            {
                func(lhs, rhs, 0);
            };

            double comp;


            result.Visit([&](auto& res)
                {
                    this->Visit([&]<typename L>(L& lhs)
                    {
                        other.Visit([&]<typename R>(R rhs)
                        {
                            res = func(lhs, rhs);

                            if constexpr (!no_limit_check)
                                comp = func((double)lhs, (double)rhs);
                        });
                    });
                });



            //Should require an integral

            if constexpr (!no_limit_check)
            {
                int overflow = result.Visit([comp]<typename T>(T res) -> int
                {
                    return  res > comp ?
                        -1 : res < comp ?
                        1 : 0;
                });


                //report::trace("overflow {}", overflow);
                //bool bound = false;

                result.NewLimitCheck(overflow);
            }

            return result;
        }


		//-end




		Number() = default;

		Number(Settings _set) :
			// _value{ v },
			_setting{ _set },
			_priority{ _setting.GetOffset() }
		{
			const char* name = "INVALID";

			Visit([&]<typename T>(T) { name = typeid(T).name(); });


			RGL_LOG(trace, "Number<{}> ctor {}, sets: sign {} size {} num {} lim {}", name,
				ToString(),
				_setting.sign, _setting.size, _setting.type, _setting.limit);
		}

		template <numeric T>
		Number(T v) :
			_setting{ Settings::CreateFromType<T>() },
			_priority{ _setting.GetOffset() }
		{

			_data.Get<T>() = v;


			RGL_LOG(trace, "Number<{}> ctor {}, sets: sign {} size {} num {} lim {}", typeid(T).name(),
				ToString(),
				_setting.sign, _setting.size, _setting.type, _setting.limit);
		}

		Number(infinity inf)
		{

			if (inf.type == NumeralType::Invalid || inf.sign == Signage::Invalid || inf.size == Size::Invalid) {
				report::fault::break_error("Number given an infinite with invalid settings. Type: {}, Sign: {}, Size: {}", (int)inf.type, (int)inf.sign, (int)inf.size);
			}

			_setting.limit = Limit::Infinite;

			*this = inf;

			RGL_LOG(trace, "Number<infinity?> ctor {}, sets: sign {} size {} num {} lim {}",
				ToString(),
				_setting.sign, _setting.size, _setting.type, _setting.limit);
		}

		Number& operator= (infinity inf)
		{
			//Infinity will completely ignore it's own settings if they're all
			auto type = inf.type != NumeralType::Invalid ? inf.type : _setting.type;
			auto sign = inf.sign != Signage::Invalid ? inf.sign : _setting.sign;
			auto size = inf.size != Size::Invalid ? inf.size : _setting.size;

			_setting.type = type;
			_setting.sign = sign;
			_setting.size = size;

			//You can't set things to infinity (yet) because it doesn't have settings. Once I make it have some settings,
			// just the non-limit ones I'll make it handle that.

			if (inf.positive) {
				_data = GetNumberLimit(type, sign, size).posInf;
			}
			else {
				_data = GetNumberLimit(type, sign, size).negInf;
			}

			if (_data._raw != 0 && _setting.limit == Limit::Infinite)
				infinite = inf.positive ? InfiniteState::Positive : InfiniteState::Negative;

			_priority = _setting.GetOffset();

			return *this;
		}


		template <numeric T>
		T Get() const
		{
			return Visit([](auto it)->T {return static_cast<T>(it); });
		}

		template <numeric T>
		bool As(T& value) const
		{
			constexpr NumberDataType type = std::is_floating_point_v<T> ?
				NumberDataType::Float : std::is_unsigned_v<T> ?
				NumberDataType::UInt : NumberDataType::SInt;

			if (GetNumberType() != type)
				return false;

			Visit([](auto it) {return value = static_cast<T>(it); });

			return true;
		}

        template<numeric T>
        operator T() const
        {
            return Get<T>();
        }





		NumberData _data;
		Settings _setting;
		uint8_t _priority;//Space is free so I might as well
		InfiniteState infinite = InfiniteState::Finite;//If active, it acts as infinity. If it's tried to transfer into
	};


    /*
    template<numeric T>
    AbstractTypePolicy* GetVariableType()
    {


        static AbstractTypePolicy* result = nullptr;

        if (!result) {

            constexpr auto setting = LEX::Number::Settings::CreateFromType<T>();


            auto buffer = LEX::IdentityManager::instance->GetTypeByOffset("NUMBER", setting.GetOffset());

            result = buffer->FetchTypePolicy(nullptr);

            logger::info("id? {}", (int)result->FetchTypeID());
        }

        return result;
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
			return fmt::format_to(ctx.out(), "{}", num.ToString());;
		}
	};
}



#pragma once

#include "Overload.h"
#include "OverloadKey.h"

#include "QualifiedType.h"

namespace LEX
{
	//TODO: Move these to their own space, or just merge them with OverloadClause. Don't really care which.
	ENUM(WriteType)
	{
		//Mutable,	//Mutable isn't stored, instead it is a modifier that determents how the below are seen.
		Modable,	//With mutable, even through the lens of const,
			Const,
			Readonly,	//Only used on functions
	};


	inline WriteType GetWriteType(Qualifier q)
	{
		//This should just put things in a way I can use easier and filter out the shit I don't need.

		switch (q)
		{
		//case Qualifier::Modable:
		//case Qualifier::Mutable:
		case Qualifier::Const:
			return WriteType::Const;

		default:
			return WriteType::Modable;
		}
	}


	inline int CompareConstness(Qualifier lhs, Qualifier rhs)
	{
		WriteType left = GetWriteType(lhs);
		WriteType right = GetWriteType(rhs);


		return right - left;
	}

	inline size_t Alter(size_t i)
	{
		//A temporary function that says I want to shift the bits before adding it. This lends less importance to some changes than others, and prevents more resolvable ambiguities.
		return i;
	}




	//2 clauses with identical keys may co-exist as long as their options are differing, as well as their use
	// the caveat is that it will produce a warning
	struct OverloadClause : public OverloadKey
	{

		//All functions and all generic entities (So stuff that comes with ISomething) should derived from overload clause in some way.
		// The interface types can merely make it a pure function to be implemented.

	public:
		//TODO: Fun fact at this point OverloadClauses and OverloadKeys have no earthly reason to exist anymore. Combine them, and sort out what to call them.
		
		OverloadClause* AsClause() override { return this; }

		//Change name to match overload.
	

		size_t GetRequiredArgs()
		{
			return 0;
		}
		
		virtual Overload __TN_Matching(QualifiedType& type, size_t& index, size_t offset) = 0;
		
		//The index will be modified to be what the true index is post use.
		Overload MatchInput(QualifiedType& type, size_t& index, size_t& offset)
		{
			//The index is adjusted to be used for default options, to denote what value it's index was.


			//Psuedo code.

			if (index == -1)
			{
				//Is a named argument. Turn it into one then find the name. For now, failure.
				return Overload::Failure();
			}

			size_t req = GetNumOfInputs().first;


			if (req <= index) {
				offset++;
			}
			return __TN_Matching(type, index, offset);
		}

		Overload MatchKey(OverloadKey* key)
		{

			if (!this || !key) {
				return Overload::Failure();
			}


			Overload result;


			int cmp = CompareConstness(GetQualifiers(), key->GetQualifiers());
			

			if (cmp < 0)
			{
				return result.Unmatch();
			}

			result += cmp;


		


			auto this_tar = GetTarget();
			auto other_tar = key->GetTarget();
			
			//This arrangement is pretty scuffed, terribly. But it works for now.
			if (static_cast<bool>(this_tar) != static_cast<bool>(other_tar))
			{
				//I didn't want to do static matching, but I sorta gotta due to the fact that extension methods are a thing.
				result += Alter(1);
			}
			else if (other_tar && this_tar) {
				//Both have to exist in order for this to count.
				if (other_tar->IsConvertibleTo(this_tar) == false)
					return Overload::Failure();
			}
			
			//use list only handles the fillings from the required, it is then upto the key to handle the same name popping up multiple times
			// within its own placement. This is because multiple strings can be in the same area denoting a specification for named param arguments.
			std::vector<bool> use_list{};

			auto input_sizes = GetNumOfInputs();

			if (key->GetNumOfInputs().first < input_sizes.first) {
				logger::critical("Big ol failure, too few arguments for overload");
				return Overload::Failure();
			}

			use_list.resize(input_sizes.first);




			auto sizes = key->GetNumOfInputGroups();



			size_t index = 0;//Manipulate this instead

			for (int x = 0; x < sizes.first; x++)
			{
				std::vector<QualifiedType> list = key->GetRequiredInput(x);

				size_t size = list.size();
				//could iterate through the list with the alternative for loop

				for (int y = 0; y < size; ++y && ++index)
				{
					size_t _x = x;
					//size_t _y = y;

					size_t i = index;
					logger::info("Some name {}", index);

					Overload overload = MatchInput(list[y], i, _x);

					if (!overload)
						return overload;

					result += overload;

					//*NOTE: The use of this is to remember the index that's being used. Thing is, the index being used isn't y, because y is the index of the param group,
					// not of the entire call.
					//int num = 0;
					//use_list[num] = true;
				}

			}

			for (int x = 0; x < sizes.first; x++)
			{
				//Don't want to waste my time with the type.

				//So here's the last thing
				auto list = key->GetOptionalInput(x);

				size_t size = list.size();



				for (int y = 0; y < size; y++)
				{
					size_t _x = x;
					size_t _y = -1;

					auto overload = MatchInput(list[y], _y, _x);
					
					if ( !overload)
						return overload;

					if (use_list[_y])
						return Overload::Failure();

					result += overload;

				}

			}

			logger::info("resulting par {}", result.par);

			return result;
		}


	};

	/*
	struct _TypeA
	{

	};
	struct _TypeB : public _TypeA
	{

	};
	struct _Type1
	{

	};
	struct _Type2 : public _Type1
	{
		operator int()
		{
			return 0;
		}

		operator std::string()
		{
			return "";
		}
	};
	struct _Type : public _TypeB, public _Type2
	{

	};


	void Call(int, _TypeA&)
	{

	}


	void Call(std::string, _TypeA&)
	{

	}

	template<typename T>
	void Call(T, _TypeA&)
	{

	}

	void Call(unsigned) {}
	void Call(float) {}

	template <typename... Use, typename A, typename B>
	void Call_Temp(A, B)
	{

	}


	template <typename... Use, typename A, typename B>
	void Call_Temp()
	{

	}

	void Test()
	{
		_Type type;

		Call(type, type);

		//Call(1);

		//Call_Temp<int, int, int, int, >();
		Call_Temp<int>(1, 2);
	}


	//*/
}
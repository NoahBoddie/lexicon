#pragma once

#include "Overload.h"
#include "OverloadKey.h"

#include "QualifiedType.h"

namespace LEX
{

	//2 clauses with identical keys may co-exist as long as their options are differing, as well as their use
	// the caveat is that it will produce a warning
	struct OverloadClause : public OverloadKey
	{

		//All functions and all generic entities (So stuff that comes with ISomething) should derived from overload clause in some way.
		// The interface types can merely make it a pure function to be implemented.

	public:
		OverloadClause* AsClause() override { return this; }


		Overload MatchKey(OverloadKey* key)
		{
			//This should work on a nullptr too.


			
			Overload result;

			if (!this)
				return result;


			//Note, this function is largely a work in progress, even if largely it's not needed.



			auto key_flags = key->GetQualifiers();

			switch (GetVolatility())
			{
			case Qualifier::Modable:
				if (key->IsModable() == false)//May possibly make mutable prefered, unsure.
					result += 1;
				break;

			case Qualifier::Mutable:
				if (key->IsModable() == true)
					result.Unmatch();
				else if (key->IsConst() == true)
					result += 1;
				break;

			case Qualifier::Const:
				if (key->IsConst() == false)
					result.Unmatch();

				break;
			default: //Should throw				
				report::apply::warn("Shouldn't even happen, but no volatility");
				result.Unmatch();
				break;
			}

			if (!result)
				return result;

			auto key_args = key->GetParamInput();

			auto clause_params = GetParamInput();

			if (key_args.size() != clause_params.size()) {
				return result.Unmatch();
				return result;
			}

			return result;

			std::vector<uint64_t> hash_list;
			hash_list.resize(key_args.size());

			//For now I will call it here. Whatever happens after, is what it is. The only matching that can
			// happen will be having the right amount of arguments.
		}


		size_t GetRequiredArgs()
		{
			return 0;
		}
		//The index will be modified to be what the true index is post use.
		Overload MatchInput(QualifiedType& type, size_t& index, size_t& offset)
		{
			//The index is adjusted to be used for default options, to denote what value it's index was.


			//Psuedo code.

			if (index == -1)
			{
				//Is a named argument. Turn it into one then find the name.
				return {};
			}

			size_t req = GetRequiredArgs();


			if (req <= index) {
				offset++;
			}

			//Do matching

			return {};
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

	void Test()
	{
		_Type type;

		Call(type, type);

		//Call(1);
	}


	ENUM(WriteType)
	{
		//Mutable,	//Mutable isn't stored, instead it is a modifier that determents how the below are seen.
		Modable,	//With mutable, even through the lens of const,
		Const, 
		Readonly,	//Only used on functions
	};


	WriteType GetWriteType(Qualifier q)
	{
		//This should just put things in a way I can use easier and filter out the shit I don't need.
	}


	int CompareConstness(Qualifier lhs, Qualifier rhs)
	{
		WriteType left = GetWriteType(lhs);
		WriteType right = GetWriteType(rhs);


		right - left;
	}

	size_t Alter(size_t i)
	{
		//A temporary function that says I want to shift the bits before adding it. This lends less importance to some changes than others, and prevents more resolvable ambiguities.
		return i;
	}

	Overload MatchKey(OverloadClause* clause, OverloadKey* key)
	{
		//This should work on a nullptr too.



		Overload result;
		

		int cmp = CompareConstness(clause->GetQualifiers(), key->GetQualifiers());

		if (cmp < 0)
		{
			Overload::Failure();
		}

		result += cmp;




		if (static_cast<bool>(clause->GetTarget()) != static_cast<bool>(clause->GetTarget()))
		{
			//I didn't want to do static matching, but I sorta gotta due to the fact that 
			result += Alter(1);
		}
		
		//use list only handles the fillings from the required, it is then upto the key to handle the same name popping up multiple times
		// within its own placement. This is because multiple strings can be in the same area denoting a specification for named param arguments.
		std::vector<bool> use_list{};
		
		use_list.resize(clause->_NumOfEntries());

		size_t size = key->_NumOfParamGroups();

		for (int x = 0; x < size; x++)
		{
			std::vector<QualifiedType> list = key->_GetInput(x);
			
			size_t size = list.size();

			for (int y = 0; y < size; y++)
			{
				size_t _x = x;
				size_t _y = y;

				if (auto overload = clause->MatchInput(list[y], _y, _x); !overload)
					return overload;

				//Mutate result += overload;

				//*NOTE: The use of this is to remember the index that's being used. Thing is, the index being used isn't y, because y is the index of the param group,
				// not of the entire call.
				int num = 0;
				use_list[num] = true;
			}

		}
		
		size = key->_NumOfOptGroups();

		for (int x = 0; x < size; x++)
		{
			//Don't want to waste my time with the type.

			//So here's the last thing
			auto list = key->_GetOptInput(x);

			size_t size = list.size();



			for (int y = 0; x < size; x++)
			{
				size_t _x = x;
				size_t _y = -1;

				if (auto overload = clause->MatchInput(list[y], _y, _x); !overload)
					return overload;

				if (use_list[_y])
					return Overload::Failure();

				//Mutate result += overload;

			}

		}

		return result;
	}
	//*/
}
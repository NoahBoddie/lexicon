#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/DataType.h"
#include "Lexicon/Engine/HierarchyData.h"

namespace LEX
{
	struct ITypePolicy;

	class ConcretePolicy;

	struct ObjectPolicy;

	//Give their own.

	//Rename policy type.
	enum struct TypeEnum : uint8_t
	{
		Invalid,
		Primitive,
		Intrinsic,
		//External and custom both fall under interface
		Interface,
		Data,
		Bound,
		Signature,
	};


	struct PolicyData : public HierarchyData
	{
	public:
		//Inheritance will look more like a vector of inheritance data, telling the hash from the main type, it's distance, etc etc.


		//Is an ITypePolicy for generic classes. Concrete can just submit without error, while Special needs to submit types (or it will just store it's specialized one)
		


		TypeID _id;


		std::string _name;


		DataType _dataType = DataType::Invalid;

		//Should be inherited, only one can exist.
		ObjectPolicy* policy = nullptr;

		std::string category;
		TypeOffset offset;

		//AbstractTypePolicy* extends() { return _extends; }
	};

}

#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/DataType.h"
#include "Lexicon/Engine/InheritanceTree.h"

namespace LEX
{
	
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


	struct PolicyData : public InheritanceTree
	{
	public:
		//Inheritance will look more like a vector of inheritance data, telling the hash from the main type, it's distance, etc etc.


		//Is an ITypePolicy for generic classes. Concrete can just submit without error, while Special needs to submit types (or it will just store it's specialized one)
		


		TypeID _id{};
		mutable InstanceID _instanceID = nil_instance_id;

		std::string _name;

		//std::string category;
		//TypeOffset offset;


		//Should be inherited, only one can exist.
		ObjectPolicy* policy = nullptr;


		//ObjectPolicyHandle handle{};

		DataType _dataType = DataType::Invalid;

	};

}

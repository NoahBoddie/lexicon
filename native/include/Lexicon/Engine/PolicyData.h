#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/DataType.h"
#include "Lexicon/AttributeData.h"
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
		mutable InstanceID _instanceID{};

		String _name;

		//std::string category;


		//TODO: I'd like these in a union
	
		//Should be inherited, only one can exist.
		
		union
		{
			size_t _raw{};
			ObjectPolicy* _policy;
			AttrDataBuilder _attrBuilder;

		};
		

		ObjectPolicy* GetPolicy() const
		{
			if (_dataType != DataType::Attribute)
				return _policy;

			return nullptr;
		}

		AttrDataBuilder GetAttributeBuilder() const
		{
			if (_dataType == DataType::Attribute)
				return _attrBuilder;

			return nullptr;
		}

		bool SetPolicy(ObjectPolicy* policy)
		{
			bool result = _dataType != DataType::Attribute;

			if (result) {
				_policy = policy;
			}

			return result;
		}

		bool SetAttributeBuilder(AttrDataBuilder builder)
		{

			bool result = _dataType == DataType::Attribute;

			if (result)
				_attrBuilder = builder;

			return result;
		}



		//ObjectPolicyHandle handle{};
		
		//TODO: I'd like to use TypeIndex to store category and offset, and get the offset from the type id and starting id.
		//TypeIndex index{};
		
		std::string_view category;
		TypeOffset offset =0;

		DataType _dataType = DataType::Invalid;

		

		
	};

}

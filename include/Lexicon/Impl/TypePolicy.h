#pragma once


#include "TypeID.h"
#include "Environment.h"
#include "AbstractTypePolicy.h"


//src
#include "IdentityManager.h"
namespace LEX
{
	class TypePolicy;
	class TypeInfo;

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

	struct TypePolicyData : public Environment
	{
	public:

		//Is an ITypePolicy for generic classes. Concrete can just submit without error, while Special needs to submit types (or it will just store it's specialized one)
		ITypePolicy* _extends;//Deprecated
		uint32_t _length;//It shouldn't ever get that fucking long anyhow, and if I do this I can fit the object type in there.
		//Something within should signal if this is a generic parameter
		
		

		TypeID _id;


		String name;

		//AbstractTypePolicy* extends() { return _extends; }
	};

	class TypePolicy : public AbstractTypePolicy, public TypePolicyData
	{
	public:
		using AbstractTypePolicy::AbstractTypePolicy;

		/*
		TypePolicy(std::string_view name, TypeOffset offset, TypeEnum enm, AbstractTypePolicy* ext = nullptr, Variable a_def = {})
		{
			//TODO:Multiple inheritence is a thing I wish to implement at a later point, for now TypePolicy takes one APolicy
		
			_id = IdentityManager::ClaimID(this, name, offset);
			_enum = enm;
			_extends = ext;
		
			_default = a_def;
			_default = Variable{ a_def, this };
		}
		//*/

		bool IsAbstract() const override { return false; }

		//Move this
		//virtual TypeEnum GetTypeEnum() = 0;

		
		AbstractTypePolicy* GetExtends() override
		{
			//I think I could have this in 2 forms. One where you output to an array and one where you just get at a point.
			// I kinda don't really super want this though cause I'd have to define it twice, so maybe a const vector or something?
			// I think an out vector might be the best solution. Idk.

			return _extends->GetTypePolicy(nullptr);
		}

		Variable GetDefault() override
		{
			RGL_LOG(critical, "unimplemented");
			return _default;
		}

	protected:
		void SetDefault(Variable& var) override
		{
			_default = var;
		}
	public:


		TypeID GetTypeID() const override
		{
			return _id;
		}

		//Rename to ForceTypeID, and then make set type ID the public one.
		void SetTypeID(TypeID id) override
		{
			_id = id;
		}

		std::string GetName() override
		{
			return name;
		}


		void CompileExpression(Record& ast) override
		{
			//tba
		}


		//Technically, should be in abstract, but abstract actually is supposed to handle this so shrug.
		//virtual TypeInfo* AsTypeInfo() { return nullptr; }

		//When loaded and not trival, added to a map. Should have a function called SetTypeID and that should handle
		// it. Should have a class called TypeIDHandler handle it.
	
	private:
		Variable _default;
	
	};
}
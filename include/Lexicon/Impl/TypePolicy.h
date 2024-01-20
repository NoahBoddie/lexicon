#pragma once


#include "TypeID.h"
#include "Environment.h"
#include "AbstractTypePolicy.h"



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

	enum struct DataType
	{
		Invalid,
		Class,
		Struct,
		Interface,
	};


	struct TypePolicyData
	{
	public:

		//Is an ITypePolicy for generic classes. Concrete can just submit without error, while Special needs to submit types (or it will just store it's specialized one)
		ITypePolicy* _extends;//Deprecated
		uint32_t _length;//It shouldn't ever get that fucking long anyhow, and if I do this I can fit the object type in there.
		//Something within should signal if this is a generic parameter
		
		

		TypeID _id;


		String _name;

		DataType _dataType = DataType::Invalid;

		//AbstractTypePolicy* extends() { return _extends; }
	};
	

	class PolicyBase : public virtual ITypePolicy, public Environment, public TypePolicyData
	{

		//This is a pivot for Policies, generic or otherwise to exist, without possibly something like
		// a specialization ending up in there (Seeing as they must be kept as ITypePolicy)

	public:


		TypeID GetTypeID() const override
		{
			return _id;
		}

		//Rename to ForceTypeID, and then make set type ID the public one.
		void SetTypeID(TypeID id)
		{
			_id = id;
		}

		std::string GetName() override
		{
			return _name;
		}



		//These doing this is kinda ill advised, but since the function is defined here, it's safe to do. Specially since
		// this is the final version of these functions.

		//Still, this is terrible practice and this likely needs to get changed. SetTypeID doesn't really
		// seem like it needs to stay virtual, due to no longer needing to be from ITypePolicy. So maybe change that
		// and we're in the clear?
		// Also replace ITypePolicy's use in IdentityManager, and move it over to this thing (Meaning more source files. Yay).
		//For now, this works
		PolicyBase();

		PolicyBase(uint32_t i);

		PolicyBase(std::string_view name, TypeOffset offset);



		Record* GetSyntaxTree() override
		{
			return _syntax;
		}


	protected:
		Record* _syntax = nullptr;

	};

	class TypePolicy : public PolicyBase, public AbstractTypePolicy
	{
	public:
		using PolicyBase::PolicyBase;

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

		void LoadFromRecord(Record& ast) override
		{
			_name = ast.GetTag();

			Record* settings = ast.FindChild("settings");

			if (!settings) {
				RGL_LOG(critical, "setting not found in type policy record");
				throw nullptr;
			}

			switch (Hash(settings->GetChild(0).GetTag()))
			{
			case "class"_h:			_dataType = DataType::Class; break;
			case "struct"_h:		_dataType = DataType::Struct; break;
			case "interface"_h:		_dataType = DataType::Interface; break;
			default:				RGL_LOG(critical, "improper data type found."); throw nullptr;
			}

			//Do something with generic. It's useless right now


			constexpr auto k_external = "interface";
			Record& unique_type = settings->GetChild(2);
			switch (Hash(unique_type.GetTag()))
			{
			//case "data"_h://Not allowed yet
			//case "external"://Not implemented well yet

			case "intrinsic"_h:		
				if (_dataType == DataType::Interface) {
					RGL_LOG(critical, "interfaces cannot be intrinsic/external");
					throw nullptr;
				}
				break;
			
			default:				
				RGL_LOG(critical, "PLACEHOLDER don't know how to handle unique type."); 
				throw nullptr;
			}
			
		}


		void CompileExpression(Record& ast) override
		{
			for (auto& node : ast.GetChildren())
			{
				switch (ast.SYNTAX().type)
				{
				case SyntaxType::Function:
					AddFunction(Component::Create<Function>(node));
					break;


				case SyntaxType::VarDeclare:
					AddVariable(Component::Create<Global>(node));
					break;


				default:
					RGL_LOG(critical, "unexpected syntax was invalid.");
					throw nullptr;
				}
			}
			



			//tba
			//This expects a type of
		}


		//Technically, should be in abstract, but abstract actually is supposed to handle this so shrug.
		//virtual TypeInfo* AsTypeInfo() { return nullptr; }

		//When loaded and not trival, added to a map. Should have a function called SetTypeID and that should handle
		// it. Should have a class called TypeIDHandler handle it.
	
	private:
		Variable _default;
	
	};
}
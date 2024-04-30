#pragma once


#include "ITypePolicy.h"
#include "Environment.h"
#include "PolicyData.h"
#include "OverloadClause.h"

namespace LEX
{


	class PolicyBase : public virtual ITypePolicy, public SecondaryEnvironment, public OverloadClause, public PolicyData
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

		OverloadClause* GetClause() override { return this; }

		//With no generic shit going on, this completely lacks the ability to be used.
		Qualifier GetQualifiers() const override { return Qualifier::None; }


		ITypePolicy* GetTarget() override { return nullptr; }

		DataType GetDataType() const override { return _dataType; }


		std::pair<size_t, size_t> GetNumOfInputs() const override { return {}; }

		std::pair<size_t, size_t> GetNumOfInputGroups() const override { return {}; }


		std::vector<RequiredArg> GetRequiredInput(size_t offset) const override { return {}; }

		std::vector<OptionalArg> GetOptionalInput(size_t offset) const override { return {}; }

		Overload __TN_Matching(QualifiedType& type, size_t& index, size_t offset) override
		{
			//success basically.
			return {};
		}
		//~


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




	};
}

#pragma once


#include "Lexicon/IType.h"

#include "Lexicon/Engine/Operand.h"


//*src
#include "Lexicon/Engine/SpecialTypeBody.h"
#include "Lexicon/Engine/Runtime.h"



namespace LEX
{

	struct DynamicTupleElement : public IType
	{
		//I forgot what this was exactly for or how it would work, but the concept 
		// is that you'd use this to dynamically access the exact type of whatever is in the tuple at this point.
		// in most places it will be seen as a completely regular type, however when it comes specializing other templates
		// it will use the proper type given.
		size_t index = -1;
		Operand operand;

		Type* GetTypePolicy(ITemplateBody* args) override
		{
			//This is basically how I would do it.

			auto runtime = dynamic_cast<Runtime*>(args);

			assert(runtime);

			auto tuple = dynamic_cast<SpecialTypeBody*>(runtime->GetBodyArgument(index));

			Number access = operand.GetVariable(runtime)->AsNumber();

			assert(access._setting.IsInteger() && access < 0);



			return tuple->GetArgument(access)->GetTypePolicy((ITemplateBody*)nullptr);
		}



		//This does nothing else but get specialized. All other uses should be reported.

		virtual TypeID GetTypeID() const { throw nullptr; }

		virtual DataType GetDataType() const { throw nullptr; }



		virtual std::string_view GetName() const { throw nullptr; }



		HierarchyData* GetHierarchyData() const override { throw nullptr; }


		virtual Variable GetDefault() { throw nullptr; }


		virtual void SetDefault(Variable&) { throw nullptr; }

		virtual Type* GetExtends() { throw nullptr; }

	};

}
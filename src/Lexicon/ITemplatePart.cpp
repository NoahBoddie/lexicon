#include "Lexicon/ITemplatePart.h"

#include "Lexicon/GenericArray.h"

namespace LEX
{

	std::unique_ptr<ITemplatePart> ITemplatePart::MakeGenericArray(GenericBase* ask, ITemplatePart* args)
	{
		//If no args, issue probably.

		auto size = GetSize();

		size_t proper = 0;

		GenericArray* gen_array = new GenericArray;

		std::unique_ptr<ITemplatePart> result{ gen_array };


		gen_array->_types.resize(size);

		for (int x = 0; x < size; x++)
		{
			auto& l_type = gen_array->_types[x];
			auto r_type = args->GetPartArgument(x);

			l_type = r_type->CheckTypePolicy(ask, args);

			proper += l_type->IsResolved();
		}

		return result;
	}

	std::unique_ptr<ITemplateBody> ITemplatePart::MakeGenericArray(ITemplateBody* args)
	{
		//If no args, issue probably.

		auto size = GetSize();

		GenericArray* gen_array = new GenericArray;

		std::unique_ptr<ITemplateBody> result{ gen_array };

		gen_array->_types.resize(size);

		for (int x = 0; x < size; x++)
		{
			auto type = args->GetPartArgument(x);

			gen_array->_types[x] = type->GetTypePolicy(args);
		}

		return result;
	}
}
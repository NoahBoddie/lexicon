#pragma once

#include "Lexicon/Engine/AbstractType.h"
#include "Lexicon/Engine/TemplateType.h"
#include "Lexicon/Engine/GenericBase.h"
#include "Lexicon/Engine/DynamicTupleElement.h"

namespace LEX
{

	struct TemplateTuple : public IType, public GenericBase
	{
		TemplateTuple(TemplateType* par, GenericBase* own)
		{
			parent = par;
		}

		TemplateType* parent = nullptr;
		GenericBase* owner = nullptr;
		std::vector<std::unique_ptr<DynamicTupleElement>> dynamicAccesses;

		//When searching for a part or a body, the find function will need to expand it first. I think actually, I may make those obtains virtual.
		// Correction, they're intended to be virtual. So, what I'll do is expand the packs (but I guess only if I need to) and then use them to find
		// what I'm looking for.



		void UnpackImpl2(ITemplateInsertPart* out, IType* tar)
		{
			if (tar->GetTypeID() == Tuple)//This means it's a tuple, we can use it for our aims.
			{
				//At this point it should be a special base somewhere, a lack of one is an error 100%.
				SpecialBase* spec = dynamic_cast<SpecialBase*>(tar);

				assert(spec);

				auto size = spec->GetSize();

				for (int i = 0; i < size; i++)
				{
					auto type = spec->GetArgument(i);

					out->InsertType(type);
				}
			}
			else
			{
				out->InsertType(tar);
			}
		}

		void UnpackImpl1(ITemplateInsertPart* out, ITemplatePart* args)
		{
			auto size = args->GetSize();

			for (int i = 0; i < size; i++)
			{
				IType* type = args->GetPartArgument(i);

				UnpackImpl2(out, type);
			}
		}


		GenericPartArray Unpack(ITemplatePart* args)
		{
			GenericPartArray result;

			UnpackImpl1(&result, args);

			return result;
		}

		GenericBodyArray Unpack(ITemplateBody* args)
		{
			GenericBodyArray result;

			UnpackImpl1(&result, args);

			return result;
		}


		SpecialBase* ObtainPart(GenericBase* client, ITemplatePart* args) override
		{
			//Unpack here

			auto pack = Unpack(args);

			return __super::ObtainPart(client, &pack);
		}

		SpecialBase* ObtainBody(ITemplateBody* args) override
		{
			//Unpack here

			auto pack = Unpack(args);

			return __super::ObtainBody(&pack);
		}




		BasicType* CheckTypePolicy(GenericBase* base, ITemplatePart* args) override
		{
			return dynamic_cast<BasicType*>(ObtainSpecial(args));
		}

		Type* GetTypePolicy(ITemplateBody* args)// override
		{
			return dynamic_cast<Type*>(ObtainBody(args));
		}


		std::unique_ptr<SpecialBase> CreatePart(ITemplatePart* args) override
		{

			return std::make_unique<SpecialTypePart>(this, this, args);
		}

		std::unique_ptr<SpecialBase> CreateBody(ITemplateBody* args) override
		{
			return std::make_unique<SpecialTypeBody>(this, this, args);
		}


	};

}
#pragma once


#include "Lexicon/ITemplatePart.h"

#include "Lexicon/Engine/SpecialBase.h"


namespace LEX
{
	struct GenericBase;


	struct SpecialPart : public SpecialBase, public ITemplatePart
	{
	public:
		SpecialPart(GenericBase* type, ITemplatePart* spec) : SpecialBase{ type }
		{
			auto size = spec->GetSize();

			for (auto i = 0; i < size; i++)
			{
				auto arg = spec->GetPartArgument(i);
				_types.emplace_back(arg);
			}
		}




		SpecialBase* ObtainSpec(GenericBase* ask, ITemplatePart* args);


		//This is still unhandled perhaps?
		SpecialBody* ObtainBody(ITemplateBody* args) override;

		bool TemplateMatches(ITemplatePart* args) override
		{
			//This cannot have fully defined types, so if it can be promoted there's no match
			if (args->TryPromoteTemplate() != nullptr)
				return false;

			auto size = _types.size();



			if (args->GetSize() != size)
				return false;

			for (int i = 0; i < size; i++)
			{
				auto& lhs = _types[i];

				ITypePolicy* rhs = args->GetPartArgument(i);

				if (lhs != rhs) {
					return false;
				}

			}

			return true;
		}

		ITypePolicy* GetArgument(size_t i) const override final
		{
			return _types[i];
		}

		virtual ITypePolicy* GetPartArgument(size_t i) const override final
		{
			return _types[i];
		}

		ITemplatePart* AsTemplate() override
		{
			return this;
		}

		size_t GetSize() const override final
		{
			return _types.size();
		}

	private:
		std::vector<ITypePolicy*> _types;
	};

}
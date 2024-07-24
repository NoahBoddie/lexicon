#pragma once


#include "Lexicon/ITemplateBody.h"

#include "Lexicon/Engine/SpecialBase.h"


namespace LEX
{


	struct SpecialBody : public SpecialBase, public ITemplateBody
	{
	public:
		SpecialBody(GenericBase* type, ITemplateBody* spec) : SpecialBase{ type }
		{
			auto size = spec->GetSize();

			for (auto i = 0; i < size; i++)
			{
				auto type = spec->GetBodyArgument(i);
				_types.emplace_back(type);
			}
		}

		virtual SpecialBody* ObtainBody(ITemplateBody* args) override
		{
			return this;
		}

		bool TemplateMatches(ITemplatePart* part) override
		{
			//This cannot have fully defined types, so if it can be promoted there's no match

			ITemplateBody* args = part->TryPromoteTemplate();

			if (!args)
				return false;

			auto size = _types.size();



			if (args->GetSize() != size)
				return false;

			for (int i = 0; i < size; i++)
			{
				auto& lhs = _types[i];
				AbstractTypePolicy* rhs = args->GetBodyArgument(i);

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


		virtual AbstractTypePolicy* GetBodyArgument(size_t i) const override final
		{
			return _types[i];
		}

		ITemplatePart* AsTemplate() override
		{
			return this;
		}

		virtual size_t GetSize() const override final
		{
			return _types.size();
		}

	private:
		//This isn't properly needed, what actually might be needed is just a hash of some kind.
		std::vector<AbstractTypePolicy*> _types;

	};

}
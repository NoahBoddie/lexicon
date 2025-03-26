#pragma once

#include "Lexicon/ITemplatePart.h"
#include "Lexicon/ITemplateInsertPart.h"


namespace LEX
{
	struct ITypePolicy;

	struct GenericPartArray : public ITemplatePart, public ITemplateInsertPart
	{
		GenericPartArray() = default;

		size_t GetSize() const override { return _types.size(); }

		ITypePolicy* GetPartArgument(size_t i) const override
		{
			auto type = _types[i];

			return type;
		}

		void InsertType(ITypePolicy* part) override
		{
			_types.emplace_back(part);
		}

		GenericBase* GetClient() const override
		{
			return _client;
		}

		mutable GenericBase* _client{};
		mutable std::vector<ITypePolicy*> _types;
	};

}
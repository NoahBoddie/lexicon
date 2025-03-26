#pragma once

#include "Lexicon/ITemplateBody.h"
#include "Lexicon/ITemplateInsertBody.h"

namespace LEX
{
	struct AbstractTypePolicy;

	struct GenericBodyArray : public ITemplateBody, public ITemplateInsertBody
	{

		size_t GetSize() const override { return _types.size(); }

		AbstractTypePolicy* GetBodyArgument(size_t i) const override
		{
			auto& type = _types[i];

			return type;
		}


		void InsertType(AbstractTypePolicy* body) override
		{
			_types.emplace_back(body);
		}

		GenericBase* GetClient() const override
		{
			return _client;
		}

		mutable GenericBase* _client = nullptr;

		mutable std::vector<AbstractTypePolicy*> _types;
	};


}
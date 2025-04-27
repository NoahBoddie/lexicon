#pragma once


#include "Lexicon/ITemplateBodyPart.h"
#include "Lexicon/ITemplateInserter.h"

namespace LEX
{

	class GenericArray : public ITemplateBodyPart, public ITemplateInserter
	{
	public:
		
		GenericArray() = default;
		GenericArray(GenericBase* base, size_t size) : _client{ base }, _types{ size } { }
		GenericArray(GenericBase* base, const std::vector<ITypeInfo*>& args) : _client{ base }, _types{ args } { }

		//This type will likely protected inherit from ITemplateBody, instead relying on the promote function to work with it?

		//I want to have this be conjoined, instead pivoting what functions it has based on whether it can promote.
		size_t GetSize() const override { return _types.size(); }


		

		ITypeInfo* GetPartArgument(size_t i) const override
		{
			return _types[i];
		}

		TypeInfo* GetBodyArgument(size_t i) const override
		{
			if (GetState())
			{
				auto& type = _types[i];

				return type->FetchTypePolicy(nullptr);;
			}

			return nullptr;
		}


		void InsertType(ITypeInfo* body) override
		{
			//Update right here.
			_types.emplace_back(body);
		}

		void tmp_UpdateResolution() const
		{
			for (auto type : _types)
			{
				if (type->IsResolved() == false) {
					isResolved = false;
					return;
				}
			}

			isResolved = true;
		}

		bool IsResolved() const
		{
			//I will only do this temporarily.
			tmp_UpdateResolution();
			return isResolved;
		}

		GenericBase* GetClient() const override
		{
			return _client;
		}

		

		mutable GenericBase* _client = nullptr;

		//TODO: Allow this to be created via span, but able to transform into a vector if it begins inserting types.
		// I think doing this will actually make setting this up a fair bit easier.
		mutable std::vector<ITypeInfo*> _types;

		//mutable for now. Should be non-mutable once types is closed off
		mutable bool isResolved = true;
		//std::variant<std::vector<ITypeInfo*>
	};

}
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

		bool Update() const
		{
			for (; updateLine < _types.size(); updateLine++)
			{
				auto type = _types[updateLine];

				if (!type) {
					return false;
				}

				if (type->IsResolved() == false) {
					updateLine = -1;
					ResetState();
					return false;
				}
			}

			return updateLine != -1;
		}


		bool ShouldUpdateState() const override
		{
			return updateLine < _types.size();
		}

		bool IsResolved() const
		{
			return Update();
		}

		GenericBase* GetClient() const override
		{
			return _client;
		}

		

		mutable GenericBase* _client = nullptr;

		//TODO: Allow this to be created via span, but able to transform into a vector if it begins inserting types.
		// I think doing this will actually make setting this up a fair bit easier.
		mutable std::vector<ITypeInfo*> _types;


		mutable size_t updateLine = 0;


	};

}
#pragma once

#include "Lexicon/Interfaces/ISubdirectory.h"

#include "Lexicon/Engine/Repository.h"

namespace LEX
{
	struct ISubdirectory;

	struct Subdirectory : public Repository, public ISubdirectory
	{
		DECL_IMPL_FUNC_REPOSITORY;

		DEFINE_COMPONENT_OFFSET(ComponentType::Subdirectory)


	protected:


		ELEM_ENUM
		{
			ELEM_FLAG(kIsSubproject, 0),

			ELEM_NEXT,
		};


	public:

		std::string_view GetName() const override
		{
			return _name;
		}

		void SetName(const std::string_view& name)
		{
			_name = std::string{ name };
		}

		SyntaxRecord* GetSyntaxTree() override
		{
			return nullptr;
		}

		void SetSyntaxTree(SyntaxRecord&) override
		{
			//Cannot set this
		}
		void SetParent(Directory* parent) override;

		Directory* GetParentImpl() override
		{
			return _parent;
		}

		const Component* GetComponent() const override
		{
			return this;
		}

		Environment* GetEnvironmentImpl() override
		{
			if (_parent)
				_parent->GetEnvironment();
			
			return nullptr;
		}

		bool IsSubproject() const
		{
			return GetFlags() & Flag::kIsSubproject;
		}

		Script* GetParentScript();


	private:
		Directory* _parent = nullptr;
		std::string _name;
	};
}
#pragma once

#include "Lexicon/Global.h"
#include "Lexicon/Interfaces/VarInfo.h"
#include "Lexicon/Engine/Field.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/GlobalData.h"
#include "Lexicon/Engine/AttributeHandler.h"
#include "Lexicon/Engine/DestructibleVarInfo.h"

namespace LEX
{
	struct IExecutableUnit;

	
	struct GlobalBase : public SecondaryElement, public GlobalData, private DestructibleVarInfo, public AttributeHandler
	{
		friend class InfoTraits;
		DEFINE_INFO_TYPE(InfoType::GlobalBase)
		DEFINE_COMPONENT_TYPE(ComponentType::GlobalBase)

	public:
		
		virtual IGlobal* AsGlobal() = 0;
		virtual const IGlobal* AsGlobal() const = 0;

		//Attempts to revert value. If the global is const or a special part, it will fail to revert.
		// If reverted just with default it will create the default value, if not, it will attempt to use a routine to set
		// information. Will not throw.
		virtual bool Revert(bool just_default) = 0;


		//I may need a new type of linker for this, so the very most default value can be set for globals, then I can use them
		// in functions.
		//Actually, it would probably be something like external would be post definition. But on definition, simple values are filled.
		// to prevent error when trying to use them.

		void LoadFromRecord(SyntaxRecord& target) override;

		void OnAttach() override;

		virtual LinkResult OnLink(LinkFlag flags) override;

		virtual LinkFlag GetLinkFlags() override;


		void HandleAttributes() override
		{
			SyntaxRecord* syntax = GetSyntaxTree();
			if (syntax) {
				if (SyntaxRecord* attribute_record = syntax->FindChild(parse_strings::attributes))

					return LoadAttributes((Component*)this, GetParent(), *attribute_record);
			}
		}


#pragma region VariableInfo

		virtual std::string_view GetName() const = 0;

		ITypeInfo* GetType() const override
		{
			return _declared.policy;
		}

		Refness GetRefness() override
		{
			return _declared.reference;
		}
		Constness GetConstness() override
		{
			return _declared.constness;
		}
		QualifierFlag GetQualifierFlags() override
		{
			return _declared.Qualifier::flags;
		}

		//virtual Specifier GetSpecifiers() const override
		//{
		//	return _declared.CopyWithFlags(SpecifierFlag::Static);
		//}
#pragma endregion

		
		DestructibleVarInfo* AsInfo() noexcept { return this; }
		const DestructibleVarInfo* AsInfo() const noexcept { return this; }
		

		void Destroy() override
		{
			return delete this;
		}
	};




	//For what it's worth, I really fucking loathe this system all together.
	template <typename T>
	struct PivotGlobalBase : public GlobalBase, public T
	{
		using GlobalBase::GlobalBase;

		IGlobal* AsGlobal() override { return this; }
		const IGlobal* AsGlobal() const override { return this; }


		std::span<AttributeBase*> GetAttributes() override
		{
			return AttributeHandler::GetAttributes();
		}


		std::string_view GetName() const override
		{
			return _name;
		}
	};

	using GenericGlobalBase = PivotGlobalBase<IGlobal>;
	using ConcreteGlobalBase = PivotGlobalBase<Global>;

}
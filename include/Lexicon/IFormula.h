#pragma once

#include "Lexicon/IExecutableUnit.h"
#include "Lexicon/Interfaces/IComponent.h"

namespace LEX
{
	struct FormulaHandler;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IFormula, IExecutableUnit)
			{
				virtual std::string_view GetName() const = 0;
				virtual void SetName(const std::string_view& name) = 0;
				
			protected:
				virtual void ModRefCountImpl(bool inc) const noexcept = 0;

			};
		}

		CURRENT_VERSION(IFormula, 1);
	}

	struct IMPL_VERSION(IFormula, IComponent) 
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IFormula, ComponentType::BasicFormula);
	public:
		void ModRefCount(bool inc, Badge<FormulaHandler>) const noexcept { ModRefCountImpl(inc); }

	};
}
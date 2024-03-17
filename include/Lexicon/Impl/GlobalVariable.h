#pragma once


#include "Field.h"
#include "Element.h"
#include "IVariable.h"

namespace LEX
{

	struct Global :
		public SecondaryElement,
		public IVariable,
		public Field//Might be more here, dunno.
	{
		std::string name;

		//Determines if this is a literal, constant, or property. The 2 former of cannot be changed, while the later can.
		//For now though, all of these will be constant.
		bool constant{};//SHould be qualifier



		virtual FieldType GetFieldType() const
		{
			//I'll come back to this, but globals will have a wrapper like functinos do. Dunno right now.
			return FieldType::Invalid;
		}

		virtual size_t GetFieldIndex() const
		{
			return max_value<size_t>;
		}

		virtual ITypePolicy* GetTypePolicy() const 
		{ 
			return GetPolicy(); 
		}
		
		BasicQualifier GetBasicFlags() const override
		{
			return {};
		}

		RuntimeQualifier GetRuntimeFlags() const override
		{
			return {};
		}

		std::string GetName() override
		{
			return name;
		}

		//Move this below please.
		std::strong_ordering operator<=>(const Global& a_rhs) const
		{
			//Too lazy to set this up right now.
			return std::strong_ordering::equal;
		}


	};
}
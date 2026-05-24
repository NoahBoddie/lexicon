#pragma once

#include "Lexicon/ITypeInfo.h"

namespace LEX
{
	struct TypeInfo;
	class Variable;
	struct ITemplateBody;
	
	//ISpecial will need to derive from interface. This interface will then also need to exist in ITypeInfo.
	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(TypeInfo)
			{
				virtual Variable GetDefault() const = 0;
				virtual Variable GetVariable() const = 0;
				
				virtual LEX::TypeInfo* GetExtends() = 0;

				virtual bool Convert(const Variable& from, Variable& to, TypeInfo* null_override = nullptr) const = 0;
			};

			
		}

		CURRENT_VERSION(TypeInfo, 1);
	}



	
	struct __declspec(novtable) IMPL_VERSION_DERIVES(TypeInfoAbstract, TypeInfo, ITypeInfo)
	{	
	
	};

	struct ABS_VERSION(TypeInfo, ITypeInfo)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::TypeInfo)
	public:

	};


#ifndef LEX_SOURCE
	//Only accessible outside of the source.

	struct IMPL_A_VERSION(TypeInfo){};
#endif


}


#ifdef LEX_SOURCE
#include "Lexicon/Engine/TypeInfoImpl.h"
#endif



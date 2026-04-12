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
				virtual Variable GetDefault() = 0;
				
				virtual LEX::TypeInfo* GetExtends() = 0;

				virtual bool Convert(const Variable& from, Variable& to, TypeInfo* null_override = nullptr) const = 0;
			};

			
		}

		CURRENT_VERSION(TypeInfo, 1);
	}



	
	struct __declspec(novtable) IMPL_VERSION_DERIVES(TypeInfoAbstract, TypeInfo, ITypeInfo)
	{	
		DEFINE_COMPONENT_OFFSET(ComponentType::TypeInfo)


		//This might come with hierarchy data automatically.


		//less of the idea of a type policy, and more an actual type policy. While this class cannot be instantiated, but the functionality of this
		// object is equal to that of a type policy, whatever it actually is.

		//TODO: ABSOLUTELY change the name "TypeInfo::IsAbstract". Perhaps "IsImplemented". Which this should never not be implemented.
		// Also, probably base that off something like the interface this comes from.

		TypeInfo* GetTypeInfo(ITemplateBody* args) override
		{
			return reinterpret_cast<TypeInfo*>(this);
		}


		
		virtual Variable GetDefault() = 0;
		
	public:
		//Make these work like an actual emplace function, where it basically does the construction elsewhere.
		// Move into function base? Largely non-basic structures depend on something else for this shit.
		//REMOVE THIS PLEASE.
		
		virtual TypeInfo* GetExtends() = 0;

		
	};


#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct TypeInfo : public TypeInfoAbstract {};
#endif


}


#ifdef LEX_SOURCE
#include "Lexicon/Engine/TypeInfoImpl.h"
#endif



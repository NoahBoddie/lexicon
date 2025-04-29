#pragma once

#include "Lexicon/ITypeInfo.hpp"

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

			protected:
				//TODO: SetDefault does NOT need to exist at all. Whatever can use this likely can just set it. Rather, move it to the TypeBase
				virtual void SetDefault(Variable&) = 0;


				

			};
		}

		CURRENT_VERSION(TypeInfo, 1);
	}


	struct IMPL_VERSION(TypeInfo), public ITypeInfo
	{	
		//This might come with hierarchy data automatically.


		//less of the idea of a type policy, and more an actual type policy. While this class cannot be instantiated, but the functionality of this
		// object is equal to that of a type policy, whatever it actually is.

		//TODO: ABSOLUTELY change the name "TypeInfo::IsAbstract". Perhaps "IsImplemented". Which this should never not be implemented.
		// Also, probably base that off something like the interface this comes from.

		TypeInfo* GetTypePolicy(ITemplateBody* args) override
		{
			return this;
		}


		
		virtual Variable GetDefault() = 0;
		
	protected: 
		//TODO: SetDefault does NOT need to exist at all. Whatever can use this likely can just set it. Rather, move it to the TypeBase
		virtual void SetDefault(Variable&) = 0;
	public:
		//Make these work like an actual emplace function, where it basically does the construction elsewhere.
		// Move into function base? Largely non-basic structures depend on something else for this shit.
		//REMOVE THIS PLEASE.
		void EmplaceDefault(Variable& var);
		void EmplaceDefault(Variable&& var) { return EmplaceDefault(var); }

		virtual TypeInfo* GetExtends() = 0;

		
	};

}
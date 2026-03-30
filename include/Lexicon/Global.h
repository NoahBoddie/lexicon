#pragma once


#include "Lexicon/IGlobal.h"


#include "Lexicon/RuntimeVariable.h"

namespace LEX
{
	class RuntimeVariable;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION_DERIVES(Global, ISpecial)
			{
				virtual const RuntimeVariable GetValue() const = 0;
				virtual bool Set(const RuntimeVariable& value) = 0;
			};
		}

		CURRENT_VERSION(Global, 1);
	}



	struct __declspec(novtable) IMPL_VERSION_DERIVES(GlobalAbstract, Global, IGlobal)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Global)
	public:
		virtual const RuntimeVariable GetValue() const = 0;
		RuntimeVariable GetValue() { return make_const(this)->GetValue(); }


		//Allows for the setting of the actual global space. If the setting defies the qualifiers it may fail.
		

		Global* GetGlobal(ITemplateBody*) override
		{
			return reinterpret_cast<Global*>(this);
		}
	};



#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct Global : public GlobalAbstract {};
#endif
} 



#ifdef LEX_SOURCE
#include "Lexicon/Engine/GlobalImpl.h"
#endif
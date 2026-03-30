#pragma once

#include "Lexicon/IFunction.h"



#include "ICallableUnit.h"

namespace LEX
{
	struct RoutineBase;
	struct ITemplateBody;
	struct TypeInfo;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(Function)
			{

			};
		}

		CURRENT_VERSION(Function, 1);
	}
	


	struct __declspec(novtable) IMPL_VERSION_DERIVES(FunctionAbstract, Function, IFunction), public ICallableUnit
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Function)
	public:

		bool IsResolved() const override { return true; }

		bool IsGeneric() const override { return false; }



		//Change this when generic gets into the picture
		Function* GetFunction(ITemplateBody* args) override
		{
			return reinterpret_cast<Function*>(this);
		}
	};



#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct Function : public FunctionAbstract {};
#endif
}



#ifdef LEX_SOURCE
#include "Lexicon/Engine/FunctionImpl.h"
#endif
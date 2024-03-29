#pragma once

#include "Element.h"
#include "IFunction.h"
#include "FunctionData.h"
#include "OverloadClause.h"
namespace LEX
{

	class FunctionBase : public virtual IFunction, public SecondaryElement, public OverloadClause, public FunctionData
	{
		//This is a pivot for for functions, more important than anywhere else, this set up excludes formulas
		// from being able to be stored in a function, or having the same linking

		void LoadFromRecord(Record& target) override;

		void OnAttach() override;

		//This and generic don't care which it is, but concrete policy will, so it's virtual to report that possibility
		// The same may be done for parameter. We shall see.
		virtual void SetReturnType(QualifiedType type);

		OverloadClause* GetClause() { return this; }



		//I haven't worked on setting up 
		Qualifier GetQualifiers() const override { return Qualifier::None; }



		ITypePolicy* GetTarget() override { return GetTargetType(); }

		std::vector<ParamInput> GetParamInput() override 
		{ 
			return  std::vector<ParamInput>{ parameters.begin(), parameters.end() }; 
		}

		//~

	public:

		bool IsMethod() const override { return _targetType; }

		std::string GetName()
		{//would an empty check be better?
			if (_name == "")
				return "<empty>";
			
			return _name;
		}
	};


}

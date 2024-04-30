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


	
		//static_assert(false, "These are without definition. Fix this shit please.");
		std::pair<size_t, size_t> GetNumOfInputs() const override { return {parameters.size(), 0}; }

		std::pair<size_t, size_t> GetNumOfInputGroups() const override { return {1, 0}; }


		std::vector<RequiredArg> GetRequiredInput(size_t offset) const override { 
			std::vector<RequiredArg> result{ parameters.size() };

			std::transform(parameters.begin(), parameters.end(), result.begin(), [&](const ParameterInfo& it) {return it.GetQualifiedType(); });

			logger::critical("delete me: size {}", result.size());
			return result;
		}

		std::vector<OptionalArg> GetOptionalInput(size_t offset) const override { return {}; }


		Overload __TN_Matching(QualifiedType& from, size_t& index, size_t offset) override
		{
			Overload result;

			if (offset)
				return Overload::Failure();

			if (parameters.size() <= index)
				return Overload::Failure();

			QualifiedType to = parameters[index].GetQualifiedType();

			if (from.IsConvertToQualified(to) == false)
				return Overload::Failure();

			return result;
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

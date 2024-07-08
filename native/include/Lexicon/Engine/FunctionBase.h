#pragma once

#include "Lexicon/Interfaces/Element.h"
#include "Lexicon/IFunction.h"
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
		
		virtual LinkResult OnLink(LinkFlag flags) override;

		virtual LinkFlag GetLinkFlags() override;

	public://This public wasn't here before. I wish to understand why it needs to be here now.

		//This and generic don't care which it is, but concrete policy will, so it's virtual to report that possibility
		// The same may be done for parameter. We shall see.
		virtual void SetReturnType(QualifiedType type);

		OverloadClause* GetClause() override { return this; }

#pragma region Clause

		bool CanMatch(QualifiedType type, size_t suggested, size_t optional, OverloadFlag flag) override
		{
			if (type) {
				if (type != _returnType)
					return false;
			}

			if (flag & OverloadFlag::UsesDefault)
			{
				logger::info("uses defaults");
				return false;
			}

			if (optional)
			{
				logger::info("uses optionals");
				return false;
			}

			if (parameters.size() - HasTarget() != suggested) {
				logger::info("uses param diff {} vs {}", parameters.size() - HasTarget(), suggested);
				return false;
			}
			return true;
		}


		//Fuck it, these return non-booleans and use something else to denote their failures.

		OverloadEntry MatchSuggestedEntry(QualifiedType type, ITypePolicy* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			OverloadEntry result;

			//TODO: This is very temp, the index can exceed the param size please remove this when params keyword is implemented
			if (index != -1 && index >= parameters.size())
			{
				logger::critical("Failure to evaluate");
				flags |= OverloadFlag::Failure;
				return {};
			}


			//I'd maybe like to rework this VariableInfo to work like this.
			//ParameterInfo* subject = index == -1 ? &thisInfo : &parameters[index];
			ParameterInfo* subject = index != -1 ? 
				&parameters[index + HasTarget()] : HasTarget() ? 
				&parameters[0] : nullptr;

			QualifiedType sub_type = subject->FetchQualifiedType();

			if (type)
			{
				LEX::Conversion* out = nullptr;//Is entries if it's not the thing. Currently, not setting this up.
				//TODO: This returns the wrong value rn.
				
				ConvertResult convertType = type.IsConvertToQualified(sub_type, scope, out);
				
				result.convertType = convertType;

				if (convertType <= ConvertResult::Failure)
				{					
					flags |= OverloadFlag::Failure;
					return result;
				}


				//result.convertType = ConvertResult::TypeDefined;
				result.index = subject->GetFieldIndex();
				result.type = sub_type;
			}
			else
			{
				result.convertType = ConvertResult::Ineligible;
				result.index = -1;
			}

			return result;

		}
		OverloadEntry MatchDefaultEntry(QualifiedType type, ITypePolicy* scope, std::string name, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return { };
		}

		std::vector<OverloadEntry> ResolveEntries(Overload& entries, ITypePolicy* scope, OverloadFlag& flags) override
		{
			return {};
		}

#pragma endregion

		//~

	public:

		bool IsMethod() const override { return _targetType; }

		std::string_view GetName() const override
		{//would an empty check be better?
			if (_name == "")
				return "<empty>";
			
			return _name;
		}


		virtual uint64_t GetProcedureData() const
		{
			return procedureData;
		}

		virtual void SetProcedureData(Procedure proc, uint64_t data)
		{
			if (data == 0 || data == -1)
				report::fault::error("Procedure data cannot be {}", static_cast<int64_t>(data));

			else if (procedureData && procedureData != -1)
				report::fault::warn("Procedure data already has value");
			_procedure = proc;
			procedureData = data;
		}


	protected:
		//If not 0, this is a procedure. This data is used by the procedure. May use to help handle if 
		//If -1 its likely waiting for a procedure to claim it.
		uint64_t procedureData = 0;

	};


}

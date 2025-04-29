#pragma once

#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/IFunctionImpl.h"
#include "Lexicon/Function.h"
#include "FunctionData.h"
#include "OverloadClause.h"
namespace LEX
{


	class FunctionBase : public SecondaryElement, public OverloadClause, public FunctionData
	{
	public:
		virtual IFunction* AsFunction() = 0;
		virtual const IFunction* AsFunction() const = 0;


	private:

		//This is a pivot for for functions, more important than anywhere else, this set up excludes formulas
		// from being able to be stored in a function, or having the same linking

		void LoadFromRecord(SyntaxRecord& target) override;

		virtual LinkResult OnLink(LinkFlag flags) override;

		virtual LinkFlag GetLinkFlags() override;
	
	protected:
		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<FunctionBase>::value):
				return this;
			}

			return nullptr;
		}

	public://This public wasn't here before. I wish to understand why it needs to be here now.

		//This and generic don't care which it is, but concrete policy will, so it's virtual to report that possibility
		// The same may be done for parameter. We shall see.
		virtual void SetReturnType(QualifiedType type);

		
#pragma region Clause

		bool CanMatch(QualifiedType type, size_t suggested, size_t optional, OverloadFlag flag) override
		{
			return FunctionData::CanMatch(type, suggested, optional, flag);
		}


		//Fuck it, these return non-booleans and use something else to denote their failures.

		OverloadEntry MatchSuggestedEntry(QualifiedType type, ITypeInfo* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			return FunctionData::MatchSuggestedEntry(type, scope, offset, index, flags);

		}
		OverloadEntry MatchDefaultEntry(QualifiedType type, ITypeInfo* scope, std::string name, OverloadFlag& flags) override
		{
			return FunctionData::MatchDefaultEntry(type, scope, name, flags);
		}

		std::vector<OverloadEntry> ResolveEntries(Overload& entries, ITypeInfo* scope, OverloadFlag& flags) override
		{
			return FunctionData::ResolveEntries(entries, scope, flags);
		}

#pragma endregion

		//~

	public:


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


	//For what it's worth, I really fucking loathe this system all together.
	template <typename T>
	struct PivotFuncBase : public FunctionBase, public T
	{
		using FunctionBase::FunctionBase;

		IFunction* AsFunction() override { return this; }
		const IFunction* AsFunction() const override { return this; }


		bool IsMethod() const override { return !!_thisInfo; }

		std::string_view GetName() const override
		{//would an empty check be better?
			if (_name == "")
				return "<empty>";

			return _name;
		}


		uint64_t GetProcedureData() const override
		{
			return procedureData;
		}

		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<IFunction>::value):
				return static_cast<IFunction*>(this);
			}

			return __super::Cast(name);
		}
	};

	using GenericFuncBase = PivotFuncBase<IFunction>;
	using ConcreteFuncBase = PivotFuncBase<Function>;

}

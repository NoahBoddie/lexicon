#pragma once

#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/IFunctionImpl.h"
#include "Lexicon/Function.h"
#include "FunctionData.h"
#include "Lexicon/Engine/OverloadInfo.h"
namespace LEX
{
	class Runtime;

	class FunctionBase : public SecondaryElement, public OverloadInfo, public FunctionData
	{
	public:
		DEFINE_COMPONENT_TYPE(ComponentType::FunctionBase)



		virtual IFunction* AsFunction() = 0;
		virtual const IFunction* AsFunction() const = 0;



		std::string_view GetName() const override
		{//would an empty check be better?
			if (_name.empty() == true)
				return "<empty>";

			return _name;
		}


	protected:

		//This is a pivot for for functions, more important than anywhere else, this set up excludes formulas
		// from being able to be stored in a function, or having the same linking

		void LoadFromRecord(SyntaxRecord& target) override;


		void OnAttach() override;

		LinkResult OnLink(LinkFlag flags) override;

		bool GetValid() const override;

		LinkFlag GetLinkFlags() override;
	
		bool IsOverloadUsuable() const override
		{
			return !InvalidFlag();
		}

	protected:


	public://This public wasn't here before. I wish to understand why it needs to be here now.

		//This and generic don't care which it is, but concrete policy will, so it's virtual to report that possibility
		// The same may be done for parameter. We shall see.
		virtual void SetReturnType(QualifiedType type);

		
		RuntimeVariable BasicExecute(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def);


		bool IsConstructor() const
		{
			return GetName() == parse_strings::constructor;
		}


		FunctionNode CreateNode(ITemplatePart* part) override
		{
			return FunctionNode{ this, this, AsFunction()->CheckFunction(part) };
		}



#pragma region Clause

		bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag flags) override
		{
			return FunctionData::CanMatch(target, callArgs, tempArgs, flags);
		}

		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags) override
		{
			return FunctionData::MatchImpliedEntry(out, type, scope, overload, index, offset, flags);
		}


		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags) override
		{
			return FunctionData::MatchStatedEntry(out, type, scope, overload, name, flags);
		}


		void QualifyOverload(Overload& overload) override
		{
			return FunctionData::QualifyOverload(overload);
		}

		bool ResolveOverload(Overload& entries, OverloadFlag& flags) override
		{
			return FunctionData::ResolveOverload(entries, flags);
		}

#pragma endregion

		//~

	public:

		//bool CanStack(Element* other) override
		//{
		//	if (!other)
		//		return false;
		//	return other->As<FunctionBase>();
		//}

		virtual void SetProcedureData(Procedure proc, uint64_t data)
		{
			if (data == 0 || data == -1)
				report::fault::error("Procedure data cannot be {}", static_cast<int64_t>(data));

			else if (procedureData && procedureData != -1)
				report::fault::warn("Procedure data already has value");
			_procedure = proc;
			procedureData = data;
		}
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
		{
			return FunctionBase::GetName();
		}


		uint64_t GetProcedureData() const override
		{
			return procedureData;
		}
	};

	using GenericFuncBase = PivotFuncBase<IFunction>;
	using ConcreteFuncBase = PivotFuncBase<Function>;

}

#pragma once

#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/IFunctionImpl.h"
#include "Lexicon/Function.h"
#include "FunctionData.h"
#include "OverloadClause.h"
namespace LEX
{


	class FunctionBase : public SecondaryElement, public OverloadParameter, public FunctionData
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

		void CheckDefault(size_t index, size_t offset, OverloadFlag& flags)
		{
			if (defaultIndex <= index && !offset) {

				flags |= OverloadFlag::DefFilled;
			}
		}
		bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag flags) override
		{
			if (target) {
				logger::info("Names {} vs {}", target->GetName(), _returnType->GetName());

				if (target != _returnType)
					return false;
			}

			//This isn't necessary
			//if (flags & OverloadFlag::StatesArgument && GetArgCountReq() < callArgs) {
			//	logger::debug("uses more than required but also states");
			//	return false;
			//}


			if (flags & OverloadFlag::StatesArgument && defaultIndex == -1)// || tempArgs.second
			{
				logger::debug("uses optionals");
				return false;
			}

			auto required = GetArgCountReq();

			if (required > callArgs) {
				logger::debug("uses param diff {} vs {}", required, callArgs);
				return false;
			}

			auto max = GetArgCountMax();

			if (max < callArgs) {
				logger::debug("uses more than max {} vs {}", max, callArgs);
				return false;
			}


			return true;
		}


		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags) override
		{
			//TODO: This is very temp, the index can exceed the param size please remove this when params keyword is implemented
			if (index != -1 && GetArgCountMax() <= index)
			{
				logger::critical("Failure to evaluate");
				return false;
			}

			CheckDefault(index, offset, flags);

			ParameterInfo* subject = FindParameterByPos(index);




			QualifiedType sub_type = subject->FetchQualifiedType();



			ConversionFlag con_flags = ConversionFlag::Parameter | ConversionFlag::Template;

			if (flags & OverloadFlag::AllAccess) {
				con_flags |= ConversionFlag::IgnoreAccess;
			}



			if (type && sub_type)
			{
				/*
				if constexpr (0)
					if (auto temp = sub_type->AsTemplate()) {
						if (auto type = overload.GetManualTemplateType(temp->index); type) {
							logger::info("template type already placed");
							sub_type.policy = type;
						}
					}
				//*/

				ConvertResult convertType = type.IsConvertToQualified(sub_type, scope, (flags & OverloadFlag::NoConvert) ? nullptr : &out.convert, con_flags);

				out.convertType = convertType;

				if (convertType <= ConversionEnum::Failure) {
					return false;
				}


				//out.convertType = ConversionEnum::TypeDefined;
				out.index = subject->GetFieldIndex();
				out.type = sub_type;
			}
			else if (!sub_type && (!type || flags & OverloadFlag::TargetOpt))
			{
				//This bit will need to change, as you may be able to access static functions from a member function.
				out.convertType = ConversionEnum::Exact;
				out.index = -1;
			}
			else
			{

				out.convertType = ConversionResult::Ineligible;
				out.index = -1;
				return false;
			}

			return true;

		}

		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags) override
		{


			//I'd maybe like to rework this VariableInfo to work like this.

			//ParameterInfo* subject = index != -1 ?
			//	&parameters[index + HasTarget()] : HasTarget() ?
			//	&parameters[0] : nullptr;

			ParameterInfo* subject = FindParameter(name);

			if (!subject) {
				report::failure("Couldn't find parameter '{}'.", name);
				return false;
			}

			if (subject->IsOptional() == false) {
				report::failure("Parameter '{}' isn't optional.", name);
				return false;
			}

			QualifiedType sub_type = subject->FetchQualifiedType();



			ConversionFlag con_flags = ConversionFlag::Parameter | ConversionFlag::Template;

			if (flags & OverloadFlag::AllAccess) {
				con_flags |= ConversionFlag::IgnoreAccess;
			}



			if (type && sub_type)
			{
				//TODO: I'm not sure if I actually want to do this, C++ doesn't do it largely because it's not possible to tell which is actually prefered
				/*
				if constexpr (0)
					if (auto temp = sub_type->AsTemplate()) {
						if (auto type = overload.GetManualTemplateType(temp->index); type) {
							logger::info("template type already placed");
							sub_type.policy = type;
						}
					}
				//*/


				ConvertResult convertType = type.IsConvertToQualified(sub_type, scope, (flags & OverloadFlag::NoConvert) ? nullptr : &out.convert, con_flags);

				out.convertType = convertType;

				out.index = subject->GetFieldIndex();
				out.type = sub_type;

				if (convertType <= ConversionEnum::Failure) {
					return false;
				}

			}
			else
			{

				out.convertType = ConversionResult::Ineligible;
				out.index = subject->GetFieldIndex();
				return false;
			}

			return true;
		}


		void ResolveOverload(Overload& result, OverloadFlag& flags)
		{
			auto& call_args = result.implied;

			call_args.resize(parameters.size());


			for (auto i = defaultIndex; i < call_args.size(); i++)
			{
				auto& entry = call_args[i];

				if (entry.type) {
					continue;
				}

				report::critical("Cant handle this yet.");


				auto& param = parameters[i];


				RoutineBase* def_routine = nullptr;

				entry.routine = def_routine;
				entry.convertType = ConversionEnum::Exact;
				entry.type = param.GetQualifiedType();
				entry.index = param.GetFieldIndex();
			}
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

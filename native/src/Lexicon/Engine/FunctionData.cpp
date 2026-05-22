#pragma once

#include "Lexicon/Engine/FunctionData.h"


//I don't know which of these permits what. I'm just giving the names a place.
#include "Lexicon/Engine/MemberInfo.h"
#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/MemberPointer.h"
#include "Lexicon/Engine/GenericBase.h"
#include "Lexicon/Engine/Overload.h"
#include "Lexicon/Engine/OverloadEntry.h"
#include "Lexicon/Engine/Runtime.h"
#include "Lexicon/ProcedureData.h"

namespace LEX
{

bool FunctionData::MatchImpliedEntryConcrete(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags)
{
	//TODO: This is very temp, the index can exceed the param size please remove this when params keyword is implemented
	if (index != -1 && GetArgCountMax() <= index)
	{
		logger::critical("Failure to evaluate");
		return false;
	}

	CheckDefault(index, offset, flags);
	CheckVariadic(index, offset);

	IVarIndexInfo* subject = FindParameterByPos(index);




	QualifiedType sub_type = NULL_OP(NULL_Q(subject)->GetQualifiedType());



	ConversionFlag con_flags = ConversionFlag::Parameter | ConversionFlag::Template;

	if (flags & OverloadFlag::AllAccess) {
		con_flags |= ConversionFlag::IgnoreAccess;
	}



	if (type && sub_type)
	{
		/*
		if constexpr (0)
			if (auto temp = sub_type->AsTemplate()) {
				if (auto type = overload.GetManualTemplateType(temp->GetIndex()); type) {
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
		out.index = (size_t)subject->GetIndex() - HasTarget();

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

bool FunctionData::MatchStatedEntryConcrete(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags)
{


	//I'd maybe like to rework this VariableInfo to work like this.

	//ParameterInfo* subject = index != -1 ?
	//	&parameters[index + HasTarget()] : HasTarget() ?
	//	&parameters[0] : nullptr;

	IVarIndexInfo* subject = FindParameter(name);

	if (!subject) {
		report::failure("Couldn't find parameter '{}'.", name);
		return false;
	}

	if (subject->IsOptional() == false) {
		report::failure("Parameter '{}' isn't optional.", name);
		return false;
	}

	QualifiedType sub_type = NULL_OP(NULL_Q(subject)->GetQualifiedType());



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
				if (auto type = overload.GetManualTemplateType(temp->GetIndex()); type) {
					logger::info("template type already placed");
					sub_type.policy = type;
				}
			}
		//*/


		ConvertResult convertType = type.IsConvertToQualified(sub_type, scope, (flags & OverloadFlag::NoConvert) ? nullptr : &out.convert, con_flags);

		out.convertType = convertType;

		out.index = (size_t)subject->GetIndex() - HasTarget();
		out.type = sub_type;

		if (convertType <= ConversionEnum::Failure) {
			return false;
		}

	}
	else
	{

		out.convertType = ConversionResult::Ineligible;
		out.index = (size_t)subject->GetIndex() - HasTarget();
		return false;
	}

	return true;
}




bool FunctionData::CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag flags)
{
	if (target) {

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
		logger::trace("uses optionals");
		return false;
	}

	auto required = GetArgCountReq();

	if (required > callArgs) {
		logger::trace("uses param diff {} vs {}", required, callArgs);
		return false;
	}

	auto max = GetArgCountMax();

	if (max < callArgs) {
		logger::trace("uses more than max {} vs {}", max, callArgs);
		return false;
	}

	if (base)
	{
		/*
		if (base->GetSize() != tempArgs)
		{
			report::failure("mismatch template sizes {} vs {}", base->GetSize(), tempArgs);
			return false;
		}
		//*/
	}
	else if (tempArgs)
	{

		report::trace("Templates used on a non-template overload");
		return false;
	}

	return true;
}

bool FunctionData::MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags)
{
	if (!(flags & OverloadFlag::IsTemplate)) {
		return MatchImpliedEntryConcrete(out, type, scope, overload, index, offset, flags);
	}

	if (!base)
		return false;

	CheckDefault(index, offset, flags);



	auto& temps = base->templates();

	if (index == -1 || temps.size() <= index)
	{
		report::failure("Failure to evaluate");
		return false;
	}


	TemplateType* subject = base->GetTemplateAt(index);

	ConversionFlag con_flags = ConversionFlag::Template;

	if (flags & OverloadFlag::AllAccess) {
		con_flags |= ConversionFlag::IgnoreAccess;
	}

	if (type && subject)
	{
		ConvertResult convertType = type->IsConvertibleTo(subject, scope, nullptr, con_flags);

		out.convertType = convertType;

		if (convertType <= ConversionEnum::Failure) {
			return false;
		}


		//out.convertType = ConversionEnum::TypeDefined;
		out.index = subject->GetIndex();
		out.type = QualifiedType{ subject };
	}
	else
	{

		out.convertType = ConversionResult::Ineligible;
		out.index = -1;
		return false;
	}

	return true;

}

bool FunctionData::MatchStatedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags)
{
	if (!(flags & OverloadFlag::IsTemplate)) {
		return MatchStatedEntryConcrete(out, type, scope, overload, name, flags);
	}

	if (!base)
		return false;

	auto& temps = base->templates();

	TemplateType* subject = base->GetTemplateByName(name);

	if (!subject) {
		report::failure("Couldn't find template type '{}'.", name);
		return false;
	}

	//This bit can be homogenized
	ConversionFlag con_flags = ConversionFlag::Template;

	if (flags & OverloadFlag::AllAccess) {
		con_flags |= ConversionFlag::IgnoreAccess;
	}

	if (type && subject)
	{
		ConvertResult convertType = type->IsConvertibleTo(subject, scope, nullptr, con_flags);

		out.convertType = convertType;

		if (convertType <= ConversionEnum::Failure) {
			return false;
		}


		//out.convertType = ConversionEnum::TypeDefined;
		out.index = subject->GetIndex();
		out.type = QualifiedType{ subject };
	}
	else
	{

		out.convertType = ConversionResult::Ineligible;
		out.index = -1;
		return false;
	}

	return true;
}


bool FunctionData::ResolveOverload(Overload& result, OverloadFlag& flags)
{
	//TODO: This needs to handle generics pls
	for (auto [type, manual] : result.specialImplied)
	{
		if (!type)
			return false;
	}

	auto& call_args = result.implied;

	//careful about this resize
	//call_args.resize(parameters.size());


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
		entry.index = (size_t)param.GetIndex() - HasTarget();
	}

	return true;
}



void FunctionData::QualifyOverload(Overload& out)
{
	if (base)
		out.requiredTemplate = base->templates().size();
}




RuntimeVariable BasicCallableData::BasicExecute(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def, FunctionBody type)
{

	//if (args.size() != GetParamCount())
	if (auto [min, max] = GetParamRange(); args.size() < min || args.size() > max)
		report::apply::error("Arg size not compatible with param size ({}/{})", args.size(), parameters.size());

	report::apply::debug("Preparing Execute for: {}", self ? self->GetName() : "Nameless function"sv);

	RuntimeVariable result;


	switch (type)
	{
	case FunctionBody::Routine:
		if constexpr (1)
		{
			Runtime runtime{ *_routine, self, args, caller, body };//The creation of caller yields 2 numbers that should not exist.

			result = runtime.Run();
		}
		break;

	case FunctionBody::Procedure:
		if constexpr (1)
		{
			if (!_procedure) {
				report::apply::error("Procedure for {} is null", NULL_OP(NULL_Q(self)->GetName(), "<no name>"));
			}

			if (!self)
				report::apply::error("procedures require a function to be associated with them.");

			Variable* target = nullptr;

			ProcedureData data;

			data.runtime = caller;
			data.defOption = def;
			data.function = self;

			auto begin = args.begin();

			std::vector<Variable*> send_args;

			send_args.reserve(GetArgCount());

			if (self->IsMethod() == true)
			{
				target = args[0].Ptr();
				begin++;
			}

			std::transform(begin, args.end(), std::back_inserter(send_args), [&](auto& it) { return it.Ptr(); });

			report _{ IssueType::Runtime };

			_procedure(result, target, send_args, data);

			for (auto& rvar : args) {
				rvar.TryUpdateRef();
			}
		}
		break;
	}


	return result;
}

	RuntimeVariable BasicCallableData::BasicInvoke(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, RuntimeVariable* def,
		FunctionBody type)
	{
		//This should ensure conversion

		if (0)
		VisitParameters([&](IVarIndexInfo& param)
			{
				int i = param.GetIndex();

				if (args.size() <= i)
					return;

				auto& arg = args[i];

				ITypeInfo* type = param.GetType();

				if (!type || type->IsScriptObject() == false) {
					return;
				}


				ITemplateBody* body = NULL_OP(NULL_Q(NULL_Q(self)->GetTemplatePart())->TryResolve());


				TypeInfo* expected = type->GetTypeInfo(body);

				assert(expected);

				arg.AdjustOffset(expected);
			});

		if constexpr (0) {
			VisitParameters([&](IVarIndexInfo& param)
				{
					int i = param.GetIndex();

					if (args.size() <= i)
						return;

					auto& arg = args[i];

					TypeInfo* expected = NULL_OP(NULL_Q(param.GetType())->GetTypeInfo(nullptr));

					//If expected is the same as the other, we send it.
				


					//There should be a reference check somewhere in here, because refs cannot convert.

					if (!expected)
						report::apply::error("null parameter type in {}", param.GetName());
					//This should be done in Invoke, which has conversion checks.
					Variable check = args[i]->Convert(expected);

					if (check.IsVoid() == true)
						report::apply::error("cannot convert argument into parameter {}, {} vs {}", param.GetName(), i, i);

					args[i].Ref() = std::move(check);
				});
		}

		auto result = BasicExecute(self, body, std::move(args), nullptr, def, type);
		//verify results
		return result;
	}

}
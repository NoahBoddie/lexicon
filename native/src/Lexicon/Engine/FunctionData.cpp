#pragma once

#include "Lexicon/Engine/FunctionData.h"


//I don't know which of these permits what. I'm just giving the names a place.
#include "Lexicon/Engine/MemberInfo.h"
#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/MemberPointer.h"
#include "Lexicon/Engine/GenericBase.h"
#include "Lexicon/Engine/Overload.h"
#include "Lexicon/Engine/OverloadEntry.h"

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
			out.index = (size_t)subject->GetFieldIndex() - HasTarget();

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

			out.index = (size_t)subject->GetFieldIndex() - HasTarget();
			out.type = sub_type;

			if (convertType <= ConversionEnum::Failure) {
				return false;
			}

		}
		else
		{

			out.convertType = ConversionResult::Ineligible;
			out.index = (size_t)subject->GetFieldIndex() - HasTarget();
			return false;
		}

		return true;
	}




	bool FunctionData::CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag flags)
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
			report::failure("Templates used on a non-template overload");
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
			out.index = subject->index;
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
			out.index = subject->index;
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
			entry.index = (size_t)param.GetFieldIndex() - HasTarget();
		}

		return true;
	}



	void FunctionData::QualifyOverload(Overload& out)
	{
		if (base)
			out.requiredTemplate = base->templates().size();
	}

}
#pragma once

#include "Lexicon/Engine/FunctionData.h"


//I don't know which of these permits what. I'm just giving the names a place.
#include "Lexicon/Engine/MemberInfo.h"
#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/MemberPointer.h"


namespace LEX
{

	bool FunctionData::CanMatch(QualifiedType type, size_t suggested, size_t optional, OverloadFlag flag)
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

	OverloadEntry FunctionData::MatchSuggestedEntry(QualifiedType type, ITypePolicy* scope, size_t offset, size_t index, OverloadFlag& flags)
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

		if (type && sub_type)
		{
			ConvertResult convertType = type.IsConvertToQualified(sub_type, scope, (flags & OverloadFlag::NoConvert) ? nullptr : &result.convert);

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
		else if (!sub_type && (!type || flags & OverloadFlag::TargetOpt))
		{
			//This bit will need to change, as you may be able to access static functions from a member function.
			result.convertType = ConvertResult::Exact;
			result.index = -1;
		}
		else
		{
			result.convertType = ConvertResult::Ineligible;
			result.index = -1;
		}

		return result;

	}
	OverloadEntry FunctionData::MatchDefaultEntry(QualifiedType type, ITypePolicy* scope, std::string name, OverloadFlag& flags)
	{
		flags |= OverloadFlag::Failure;
		return { };
	}

	std::vector<OverloadEntry> FunctionData::ResolveEntries(Overload& entries, ITypePolicy* scope, OverloadFlag& flags)
	{
		return {};
	}

}
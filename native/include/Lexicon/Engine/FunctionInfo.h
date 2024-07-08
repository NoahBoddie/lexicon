#pragma once

#include "Lexicon/Engine/MemberInfo.h"
#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/MemberPointer.h"

namespace LEX
{

	struct FunctionInfo : public MemberInfo, public OverloadClause
	{
		using FunctionType = FunctionBase;//
		


		struct Newstructure
		{
			//Since this takes up the same about of space, I think what I'll do is this.
			//it holds function data

			//So the concept here is it always holds function data, and that this is the thing that's used with overloads.
			//This thing to find if something matches will try for equality.

			//the structure

			//There's a slight problem with the idea and the implementation of generics would cause me some issues. So, instead, I think it would probably be better
			// to just have it the way that I have it now, either a function or a method pointer. That being said, I can likely drop the FunctionData and just stick with the member
			// pointer, having an empty function at said location.
			// I might instead spend that information on this being able to know it's owner, OR just require that to be submitted in order to get that.

			FunctionData* signature;

			union
			{
				uint64_t raw = 0;
				MemberPointer virtualMethod;
				FunctionBase* declaredMethod;
			};

			bool isPureVirtual = false;
		};


		struct {

			FunctionData* signature = nullptr;
			

			struct
			{
				uint64_t _raw = 0;
				FunctionType* function;
				MemberPointer method;//prefered, works with the other.
				
			};
		};

		bool IsVirtual() const
		{
			return specifiers & Specifier::Virtual;
		}

		FunctionType* Get() const
		{
			if (IsVirtual() == false)
				return function;

			return nullptr;
		}

		FieldType GetFieldType() const override { return FieldType::Function; }

		//For now, this is true, there is no way to handle a function's type.
		ITypePolicy* GetType() const override { return nullptr; }


		Qualifier GetQualifiers() const override { return Qualifier::Const; }	//Functions do not have qualifiers.
		Specifier GetSpecifiers() const override { return specifiers; }

		std::string GetFieldName() const override { return std::string{ Get()->GetName() }; }

		operator bool() const override
		{
			return _raw || signature;
		}


		bool CanMatch(QualifiedType type, size_t suggested, size_t optional, OverloadFlag flag) override
		{
			if (type) {
				if (type != signature->_returnType)
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

			if (signature->parameters.size() - signature->HasTarget() != suggested) {
				logger::info("uses param diff {} vs {}", signature->parameters.size() - signature->HasTarget(), suggested);
				return false;
			}
			return true;
		}


		//Fuck it, these return non-booleans and use something else to denote their failures.

		OverloadEntry MatchSuggestedEntry(QualifiedType type, ITypePolicy* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			OverloadEntry result;

			//TODO: This is very temp, the index can exceed the param size please remove this when params keyword is implemented
			if (index != -1 && index >= signature->parameters.size())
			{
				logger::critical("Failure to evaluate");
				flags |= OverloadFlag::Failure;
				return {};
			}


			//I'd maybe like to rework this VariableInfo to work like this.
			//ParameterInfo* subject = index == -1 ? &thisInfo : &parameters[index];
			ParameterInfo* subject = index != -1 ?
				&signature->parameters[index + signature->HasTarget()] : signature->HasTarget() ?
				&signature->parameters[0] : nullptr;

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
		OverloadEntry MatchDefaultEntry(QualifiedType type, ITypePolicy* scope, std::string name, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return { };
		}

		std::vector<OverloadEntry> ResolveEntries(Overload& entries, ITypePolicy* scope, OverloadFlag& flags) override
		{
			return {};
		}


		/*

		//The concept with this is that the actual function info's themselves are 
		bool PreEvaluate(size_t suggested, size_t optional, OverloadFlag flag) override
		{
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

			if (signature->parameters.size() - signature->HasTarget() != suggested) {
				logger::info("uses param diff {} vs {}", signature->parameters.size() - signature->HasTarget(), suggested);
				return false;
			}
			return true;
		}


		OverloadEntry EvaluateEntry2(QualifiedType type, ITypePolicy* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			OverloadEntry result;

			//TODO: This is very temp, the index can exceed the param size please remove this when params keyword is implemented
			if (index != -1 && index >= signature->parameters.size())
			{
				logger::critical("Failure to evaluate");
				flags |= OverloadFlag::Failure;
				return {};
			}


			//I'd maybe like to rework this VariableInfo to work like this.
			//ParameterInfo* subject = index == -1 ? &thisInfo : &parameters[index];
			ParameterInfo* subject = index != -1 ?
				&signature->parameters[index + signature->HasTarget()] : signature->HasTarget() ?
				&signature->parameters[0] : nullptr;

			QualifiedType sub_type = subject->FetchQualifiedType();

			if (type)
			{
				LEX::Conversion* out = nullptr;//Is entries if it's not the thing. Currently, not setting this up.
				//TODO: This returns the wrong value rn.

				ConvertResult convertType = type.IsConvertToQualified(sub_type, scope, out);

				result.convertType = convertType;

				if (convertType == ConvertResult::Failure)
				{
					flags |= OverloadFlag::Failure;
					return result;
				}


				result.convertType = ConvertResult::TypeDefined;
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
		OverloadEntry EvaluateDefault2(QualifiedType type, ITypePolicy* scope, std::string name, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return { };
		}

		std::vector<OverloadEntry> GetRemainingEvals(Overload& entries, ITypePolicy* scope, OverloadFlag& flags) override
		{
			return {};
		}

		//*/


	};

}

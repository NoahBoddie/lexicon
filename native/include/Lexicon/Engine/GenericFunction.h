#pragma once

#include "Lexicon/Engine/OverloadClause.h"
#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/Engine/GenericBase.h"


namespace LEX
{



	struct GenericFunction : public GenericFuncBase, public GenericBase
	{
		GenericFunction()
		{
			base = this;
		}

		std::unique_ptr<SpecialBase> CreateSpecial(ITemplatePart* args) override;



		IFunction* CheckFunction(ITemplatePart* args) override;

		Function* GetFunction(ITemplateBody* args) override;


		ITemplatePart* GetTemplatePart() override
		{
			return this;
		}

		ISpecializable* GetSpecializable() override
		{
			return this;
		}


		void QualifyOverload(Overload& out) override
		{
			out.requiredTemplate = templates().size();
		}




		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags) override
		{
			if (!(flags & OverloadFlag::IsTemplate)) {
				return __super::MatchImpliedEntry(out, type, scope, overload, index, offset, flags);
			}

			CheckDefault(index, offset, flags);



			auto& temps = templates();

			if (index == -1 || temps.size() <= index)
			{
				report::failure("Failure to evaluate");
				return false;
			}


			TemplateType* subject = GetTemplateAt(index);

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

		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags) override
		{
			if (!(flags & OverloadFlag::IsTemplate)) {
				return __super::MatchStatedEntry(out, type, scope, overload, name, flags);
			}

			auto& temps = templates();

			TemplateType* subject = GetTemplateByName(name);

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

	};


}
#pragma once

#include "Lexicon/Engine/SyntaxRecord.h"
#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/Engine/HeaderSettings.h"


//*src
#include "Lexicon/Engine/Parser.h"

namespace LEX
{
	struct ParseUtility
	{
	private:

		static SyntaxRecord* PeekCurrentPathImpl(SyntaxRecord*& a_this)
		{

			if (a_this && a_this->IsPath() == false) {
				a_this = a_this->FindChild(parse_strings::path);
			}

			if (!a_this || a_this->IsPath() == false) {
				return nullptr;
			}

			auto identifier = a_this->FindChild(parse_strings::identifier);

			assert(identifier);

			return identifier ? &identifier->GetFront() : nullptr;
		}

		static SyntaxRecord* PeekCurrentExpressionImpl(SyntaxRecord*& a_this)
		{

			if (a_this && a_this->IsPath() == false) {
				a_this = a_this->FindChild(parse_strings::path);
			}

			if (!a_this || a_this->IsPath() == false) {
				return nullptr;
			}

			auto left = a_this->FindChild(parse_strings::lhs);

			return left ? &left->GetFront() : &a_this->GetFront();
		}

		static bool AddHeaderKeywordImpl(KeywordType type, Record& parent, Record& record, bool move)
		{
			auto header = parent.FindChild(parse_strings::header);

			assert_if(!header) {
				return false;
			}

			auto& branch = header->GetChild(type);


			if (move){
				branch.EmplaceChild(std::move(record));
			}
			else {
				branch.EmplaceChild(record);
			}

			return true;
		}

	public:

		static Record MakeHeader()
		{

			Record it = ParsingStream::CreateExpression(parse_strings::header, SyntaxType::Header, std::vector<Record>{ 3 });

			//Doing it like this ensures the order will not change.
			it.GetChild(KeywordType::TypeQual) = ParsingStream::CreateExpression(parse_strings::type_qualifier, SyntaxType::Header);
			it.GetChild(KeywordType::DeclSpec) = ParsingStream::CreateExpression(parse_strings::declare_specifier, SyntaxType::Header);
			it.GetChild(KeywordType::TypeSpec) = ParsingStream::CreateExpression(parse_strings::type_specifier, SyntaxType::Header);

			return it;
		}

		static bool AddHeaderKeyword(KeywordType type, Record& parent, const Record& record)
		{
			return AddHeaderKeywordImpl(type, parent, unconst(record), false);
		}

		static bool AddHeaderKeyword(KeywordType type, Record& parent, Record&& record)
		{
			return AddHeaderKeywordImpl(type, parent, record, true);
		}


		static SyntaxRecord* PeekCurrentPath(SyntaxRecord* a_this)
		{
			return PeekCurrentPathImpl(a_this);
		}


		static SyntaxRecord* PeekCurrentPath(SyntaxRecord& a_this)
		{
			return PeekCurrentPath(&a_this);
		}




		static SyntaxRecord& GetFrontExpression(SyntaxRecord& a_this)
		{
			switch (a_this.GetSyntax().type)
			{
			case SyntaxType::Binary:
				return *a_this.FindChild(parse_strings::lhs);

			case SyntaxType::Unary:
				return a_this.GetFront();
			default:
				return a_this;
			}
		}

		static SyntaxRecord* SeekNextPath(SyntaxRecord*& a_this)
		{
			auto current = PeekCurrentPathImpl(a_this);

			a_this = a_this->FindChild(parse_strings::path);

			return current;
		}


		static SyntaxRecord& PeekFirstRecord(SyntaxRecord& a_this)
		{
			auto record = PeekCurrentPath(a_this);

			return record ? *record : a_this;

		}
		

		//Traits are non-blocking and shouldn't be the be end be all, rather used as a header

		//I might just send these a hash code instead, and switch through it. I think for the many types this will save some time.
		

		//These will all need parse string entries

		static bool StateOrExpressTrait(const std::string_view& word)
		{
			//Literally is statement or expression so it's not ticked when just asking for one.
			return word == "statement or expression";
		}


		static bool StatementTrait(const std::string_view& word)
		{
			return word == "statement" || StateOrExpressTrait(word);
		}

		static bool ExpressionTrait(const std::string_view& word)
		{
			return word == "expression" || StateOrExpressTrait(word);
		}



		static bool DeclareTrait(const std::string_view& word)
		{
			return word == "declaration";
		}




		
		static bool DeclareStatementTrait(const std::string_view& word)
		{
			return DeclareTrait(word) || StatementTrait(word);
		}


		static bool AtomicTrait(const std::string_view& word)
		{
			return ExpressionTrait(word) || word == "atomic";
		}


		//I want a trait that poses itself as always being allowed
	};
}
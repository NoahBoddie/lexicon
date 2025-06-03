#pragma once

#include "Lexicon/Engine/SyntaxRecord.h"
#include "Lexicon/Engine/parse_strings.h"

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

			auto left = a_this->FindChild(parse_strings::lhs);
			
			return left ? &left->GetFront() : &a_this->GetFront();
		}
	public:


		static SyntaxRecord* PeekCurrentPath(SyntaxRecord* a_this)
		{
			return PeekCurrentPathImpl(a_this);
		}


		static SyntaxRecord* PeekCurrentPath(SyntaxRecord& a_this)
		{
			return PeekCurrentPath(&a_this);
		}


		static SyntaxRecord* SeekNextPath(SyntaxRecord*& a_this)
		{
			auto current = PeekCurrentPathImpl(a_this);

			a_this = a_this->FindChild(parse_strings::rhs);

			return current;
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
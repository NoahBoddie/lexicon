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
			
			return left ? left : &a_this->GetFront();
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

		
	};
}
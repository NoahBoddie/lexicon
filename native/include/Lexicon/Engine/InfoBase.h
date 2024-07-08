#pragma once


#include "Lexicon/Field.h"

namespace LEX
{
	struct Solution;


	struct InfoBase : public Field
	{
		//The abstract class for infos. Holds all the data for it.

		

		constexpr InfoBase(Qualifier q = Qualifier::None, Specifier s = Specifier::None, uint32_t i = -1) noexcept : 
			qualifiers{ q },
			specifiers{ s },
			_index{ i }
		{}

		Qualifier qualifiers;

		DeclareSpecifier specifiers;

		//Access modifiers and 


		//bool isVirtual = 0;
		//bool isIsRuntimeMember = 0;//This is actually used to tell if it should purge runtime stuff from members. I easily could do that.


		//All versions will need this at some point, so I'll do this here.
		uint32_t _index = -1;

		uint32_t GetFieldIndex() const override
		{
			return _index;
		}
		//This isn't super needed actually.
		Solution AsSolution();

		virtual operator bool() const = 0;
	};

}

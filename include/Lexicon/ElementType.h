#pragma once

namespace LEX
{
	//TODO: remove element type and use component type.
	enum ElementType : uint8_t
	{
		kNoneElement,
		kTypeElement,
		kFuncElement,
		kGlobElement,
		kScrpElement,
		//kProjElement,
	};
}
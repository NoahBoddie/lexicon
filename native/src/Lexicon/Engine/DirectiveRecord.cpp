#include "Lexicon/Engine/DirectiveRecord.h"

#include "Lexicon/Engine/Script.h"


namespace LEX
{

	BasicRecord<Directive, DirectiveBody>* DirectiveBody::GetSelf()
	{
		return reinterpret_cast<Self*>(this);
	}

	Directive& DirectiveBody::GetDirective()
	{
		return GetSelf()->GetEnumFromRecord();
	}


}
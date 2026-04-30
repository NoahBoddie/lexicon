#include "Lexicon/Interfaces/Info.h"
#include "Lexicon/Engine/Impl/InfoTraits.h"
namespace LEX
{
	const void* Info::Cast(const void* self, InfoType from, InfoType to) const
	{
		return InfoTraits::InfoCast(self, from, to);
	}
}
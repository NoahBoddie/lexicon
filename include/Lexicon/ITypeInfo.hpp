#pragma once

//A pivot file for ITypeInfo. Removes the hassle of knowing of which version to include.

#ifdef LEX_SOURCE
#include "Lexicon/Engine/ITypeInfoImpl.h"
#else
#include "Lexicon/ITypeInfo.h"
#endif

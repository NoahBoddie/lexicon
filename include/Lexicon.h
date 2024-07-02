#pragma once 


//Give to RGL

#include "RoguesGallery.hpp"

/*
//Also move to rogues gallery
#pragma push_macro("max")
#undef max

template <class T>
constexpr T max_value = std::numeric_limits<T>::max();

#pragma pop_macro("max")
//*/


#define LEX_SOURCE 1
#define LEX_API_SOURCE "test.dll"

//These don't really work for me, Id like some more order to these.
#define LEX_NAMESPACE namespace LEX

//These are shared, so rogues gallery probably uses this too.
#define LIB LEX
#define IMPL Impl

#define LIB_IMPL LIB::IMPL


//Don't think I'll be using this.
//#define IMPL_NAMESPACE_START namespace LIB_IMPL {

//#define IMPL_NAMESPACE_START }




namespace LEX
{
	using namespace RGL_NAMESPACE;
	using namespace RGL_INCLUDE_NAMESPACE;
}

//#include "Lexicon/Impl/OperandType.h"
//#include "Lexicon/Impl/Register.h"
//#include "Lexicon/Impl/Operand.h"

//Component must move from impl.
//#include "Lexicon/Impl/Component.h"

//#include "Lexicon/Impl/TestField.h"


#include "Lexicon/Impl/Native/OperandType.h"
#include "Lexicon/Register.h"
#include "Lexicon/Impl/Native/Operand.h"

//Component must move from impl.
#include "Lexicon/Component.h"

#include "Lexicon/TestField.h"


#include "Lexicon/Impl/Native/OperandType.h"

#include "Lexicon/Impl/Native/Operand.h"

//Component must move from impl.
#include "Lexicon/Component.h"

//#include "Lexicon/Impl/TestField.h"
#include "Lexicon/Impl/Native/Parser.h"
#include "Lexicon/Impl/Native/ParserTest.h"
#include "Lexicon/Impl/Native/RoutineBase.h"
#include "Lexicon/Impl/Native/RoutineCompiler.h"

//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_color_sinks.h"
//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/msvc_sink.h>
//#include <spdlog/sinks/stdout_sinks.h>


#include "Lexicon/Runtime.h"

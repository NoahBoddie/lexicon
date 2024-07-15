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

//I can do both of these at the same time.
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

//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_color_sinks.h"
//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/msvc_sink.h>
//#include <spdlog/sinks/stdout_sinks.h>


#include "Lexicon/Impl/PCH.h"

#include "Lexicon/AbstractFunction.h"
#include "Lexicon/AbstractTypePolicy.h"
#include "Lexicon/AccessModifier.h"
#include "Lexicon/Array.h"
#include "Lexicon/Component.h"
#include "Lexicon/Conversion.h"
#include "Lexicon/DataType.h"
#include "Lexicon/DeclareSpecifier.h"
#include "Lexicon/Default.h"
#include "Lexicon/Dispatcher.h"
#include "Lexicon/Exception.h"
#include "Lexicon/Field.h"
#include "Lexicon/ICallableUnit.h"
#include "Lexicon/IFunction.h"
#include "Lexicon/IGenericArgument.h"
#include "Lexicon/IGenericParameter.h"
#include "Lexicon/ISpecial.h"
#include "Lexicon/ITypePolicy.h"
#include "Lexicon/IVariable.h"
#include "Lexicon/MemberPointer.h"
#include "Lexicon/Number.h"
#include "Lexicon/Object.h"
#include "Lexicon/ObjectData.h"
#include "Lexicon/ObjectInfo.h"
#include "Lexicon/ObjectPolicy.h"
#include "Lexicon/ObjectPolicyHandle.h"
#include "Lexicon/Interfaces/ObjectPolicyManager.h"
#include "Lexicon/ObjectPoolData.h"
#include "Lexicon/QualifiedField.h"
#include "Lexicon/QualifiedType.h"
#include "Lexicon/Qualifier.h"
#include "Lexicon/Report.h"
#include "Lexicon/Engine/Runtime.h"
#include "Lexicon/RuntimeVariable.h"
#include "Lexicon/String.h"
#include "Lexicon/TestField.h"
#include "Lexicon/TypeAliases.h"
#include "Lexicon/TypeID.h"
#include "Lexicon/Unvariable.h"
#include "Lexicon/Variable.h"
#include "Lexicon/VariableType.h"
#include "Lexicon/VersionDeclareSpace.h"
#include "Lexicon/Versioning.h"
#include "Lexicon/Interfaces/Element.h"
#include "Lexicon/Interfaces/Environment.h"
#include "Lexicon/Interfaces/Project.h"
#include "Lexicon/Interfaces/Script.h"
#include "Lexicon/Interfaces/IdentityManager.h"
#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceManager.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"
#include "Lexicon/Interfaces/ProcedureHandler.h"
#include "Lexicon/Interfaces/ProjectManager.h"


#include "Lexicon/ProcedureData.h"


#ifdef LEX_SOURCE
#include "Lexicon/Engine.h"
#endif


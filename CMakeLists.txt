cmake_minimum_required(VERSION 3.21)

# ---- Project ----

project(
	Lexicon
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
			"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
)
endif()

# ---- Dependencies ----

find_package(fmt REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
#find_package(tomlplusplus REQUIRED CONFIG)
#find_package(Boost MODULE REQUIRED)

# ---- Add source files ----

set(headers 

include/Lexicon.h
#include/Lexicon/Impl/LexComponent.h
#include/Lexicon/Impl/TOMLHandler.h
#include/Lexicon/Impl/Exception.h
include/Lexicon/Impl/Parser.h
include/Lexicon/Impl/ParseModule.h
 "include/Lexicon/Impl/Field.h" "include/Lexicon/Impl/Instruction.h" "include/Lexicon/Impl/RuntimeVariable.h" "include/Lexicon/Impl/Target.h"  "include/Lexicon/Impl/InstructionType.h" "include/Lexicon/Impl/RoutineBase.h" "include/Lexicon/Impl/Solution.h" "include/Lexicon/Impl/RoutineCompiler.h" "include/Lexicon/Impl/Scope.h" "include/Lexicon/Impl/LiteralManager.h" "include/Lexicon/Impl/IVariable.h" "include/Lexicon/Impl/GlobalVariable.h" "include/Lexicon/Impl/Runtime.h" "include/Lexicon/Impl/MemberPointer.h" "include/Lexicon/Impl/OperatorType.h" "include/Lexicon/Impl/ICallableUnit.h" "include/Lexicon/Impl/Number.h"
 "include/Lexicon/Impl/Literal.h" 
 "include/Lexicon/Impl/FunctionHandle.h" 
 "include/Lexicon/Impl/Delegate.h" 
 "include/Lexicon/Impl/Array.h" 
 "include/Lexicon/Impl/IdentityManager.h" 
 "include/Lexicon/Impl/Origin.h" 
 "include/Lexicon/Impl/Report.h" 
 "include/Lexicon/Impl/BasicQualifier.h" 
 "include/Lexicon/Impl/RuntimeQualifier.h" 
 "include/Lexicon/Impl/InfoBase.h" 
 "include/Lexicon/Impl/TemporaryInfo.h" 
 "include/Lexicon/Impl/MemberInfo.h" 
 "include/Lexicon/Impl/AccessModifier.h" 
 "include/Lexicon/Impl/LocalInfo.h" 
 "include/Lexicon/Impl/ParameterInfo.h" 
 "include/Lexicon/Impl/VariableInfo.h" 
 "include/Lexicon/Impl/FunctionInfo.h" 
 "include/Lexicon/Impl/FunctionData.h" 
 "include/Lexicon/Impl/FunctionBase.h" 
 "include/Lexicon/Impl/ConcreteFunction.h" 
 "include/Lexicon/Impl/PolicyData.h" 
 "include/Lexicon/Impl/PolicyBase.h" 
 "include/Lexicon/Impl/ConcretePolicy.h" 
 "include/Lexicon/Impl/External.h" 
 "include/Lexicon/Impl/ExternalHandle.h"
 "include/Lexicon/Impl/ExternalManager.h"
 "include/Lexicon/Impl/ExternalInterface.h"
 "include/Lexicon/Impl/VariableType.h" 
 "include/Lexicon/Impl/Default.h" 
 "include/Lexicon/Impl/TargetObject.h" 
 "include/Lexicon/Impl/Overload.h" 
 "include/Lexicon/Impl/ParamInput.h" 
 "include/Lexicon/Impl/GenericInput.h"
 "include/Lexicon/Impl/OverloadKey.h" 
 "include/Lexicon/Impl/OverloadClause.h"
 "include/Lexicon/Impl/OverloadInput.h" 
 "include/Lexicon/Impl/SearchPredicate.h"  
 "include/Lexicon/Impl/DeclareSpecifier.h" 
 "include/Lexicon/Impl/VoidPolicy.h" 
 "include/Lexicon/Impl/QualifiedType.h" 
 "include/Lexicon/Impl/Qualifier.h" 
 "include/Lexicon/Impl/Object.h" 
 "include/Lexicon/Impl/ObjectPolicyHandle.h"
 "include/Lexicon/Impl/ObjectPolicy.h" 
 "include/Lexicon/Impl/ObjectPolicyManager.h" 
 "include/Lexicon/Impl/ObjectInfo.h" 
 "include/Lexicon/Impl/ObjectPoolData.h" 
 "include/Lexicon/Impl/Declaration.h" 
 "include/Lexicon/Impl/HeaderSettings.h" 
 "include/Lexicon/Impl/Interface.h" 
 "include/Lexicon/Impl/InterfaceManager.h" 
 "include/Lexicon/Impl/InterfaceSingleton.h"
 "include/Lexicon/Impl/HierarchyData.h"
 "include/Lexicon/Impl/Versioning.h"  "include/Lexicon/Impl/VersionDeclareSpace.h" "include/Lexicon/Impl/DataType.h" "include/Lexicon/Impl/QualifiedField.h" "include/Lexicon/Impl/GlobalData.h" "include/Lexicon/Impl/Conversion.h" "include/Lexicon/Impl/OverloadEntry.h" "include/Lexicon/Impl/OverloadSection.h" "include/Lexicon/Impl/OverloadFlag.h" "include/Lexicon/Impl/InheritData.h" "include/Lexicon/Impl/CompileUtility.h" "include/Lexicon/Impl/parse_strings.h" "include/Lexicon/Impl/ProcedureHandler.h" "include/Lexicon/Impl/Dispatcher.h" "include/Lexicon/Impl/Unvariable.h" "include/Lexicon/Impl/String.h")

set(sources
#src/main.cpp
"include/Lexicon/Impl/TempConstruct.h" 
src/Lexicon/Impl/Parser.cpp
src/Lexicon/Impl/ParseModule.cpp
src/Lexicon/Impl/ParseHandler.cpp
src/Lexicon/Impl/ProcessContext.cpp
src/Lexicon/Impl/InputStream.cpp
src/Lexicon/Impl/TokenStream.cpp
src/Lexicon/Impl/TokenHandler.cpp
src/Lexicon/Component.cpp
"include/Lexicon/Impl/Literal.cpp"
include/Lexicon/Impl/LiteralManager.cpp
 "src/Lexicon/Impl/Element.cpp" "src/Lexicon/Impl/Script.cpp" "src/Lexicon/Impl/ProjectManager.cpp" "src/Lexicon/Impl/Project.cpp" "include/Lexicon/Impl/Operand.cpp" "include/Lexicon/Impl/Operation.cpp"
 "include/Lexicon/Impl/Declaration.cpp"
 "include/Lexicon/Impl/TempConstruct.cpp" "include/Lexicon/Impl/TypeID.cpp" "include/Lexicon/Impl/RoutineCompiler.cpp" "include/Lexicon/Impl/ITypePolicy.cpp" "include/Lexicon/Impl/AbstractTypePolicy.cpp" "include/Lexicon/Impl/IdentityManager.cpp" "include/Lexicon/Impl/Environment.cpp" "include/Lexicon/Impl/PolicyBase.cpp" "include/Lexicon/Impl/ExternalInterface.cpp" "include/Lexicon/Impl/FunctionBase.cpp" "include/Lexicon/Impl/ConcreteFunction.cpp" "include/Lexicon/Impl/ICallableUnit.cpp" "include/Lexicon/Impl/Scope.cpp" "include/Lexicon/Impl/OverloadInput.cpp" "include/Lexicon/Impl/ParamInput.cpp" "include/Lexicon/Impl/Variable.cpp" "include/Lexicon/Impl/VariableType.cpp" "include/Lexicon/Impl/Field.cpp" "include/Lexicon/Impl/VariableInfo.cpp" "include/Lexicon/Impl/ObjectPolicyHandle.cpp" "include/Lexicon/Impl/ConcretePolicy.cpp" "include/Lexicon/Impl/InterfaceManager.cpp" "include/Lexicon/Impl/ObjectPolicy.cpp"  "include/Lexicon/Impl/ObjectPolicyManager.cpp" "include/Lexicon/Impl/QualifiedType.cpp" "include/Lexicon/Impl/OverloadKey.cpp"  "include/Lexicon/Impl/VoidPolicy.cpp" "include/Lexicon/Impl/ProcedureHandler.cpp" "include/Lexicon/Impl/Dispatcher.cpp" "include/Lexicon/Impl/Signature.h" "include/Lexicon/Impl/OverloadEntry.cpp" "include/Lexicon/Impl/HierarchyData.cpp")
 
source_group(
	TREE
		${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${headers}
		${sources}
)

source_group(
	TREE
		${CMAKE_CURRENT_SOURCE_DIR}/bignato
	FILES
)

# ---- Create library ----

include(GNUInstallDirs)


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(PUBLIC_ACCESS PUBLIC)
	set(PRIVATE_ACCESS PRIVATE)


	
	add_library(
		${PROJECT_NAME}
		#PRIVATE
		#STATIC
		${headers}
		${sources}
		.clang-format
	)
else()
	set(PUBLIC_ACCESS PUBLIC)
	set(PRIVATE_ACCESS PRIVATE)

	add_library(
		${PROJECT_NAME}
		STATIC
		${headers}
		${sources}
		.clang-format
	)
endif()


add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_sources( Lexicon PRIVATE "include/Lexicon/Impl/Default.h" )
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(
	${PROJECT_NAME}
	${PUBLIC_ACCESS}
		cxx_std_23
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/bignato/include") 



#I moved this in myself
target_include_directories(${PROJECT_NAME}
        ${PRIVATE_ACCESS}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
        $<INSTALL_INTERFACE:src>)

#interface

#was explicitly public, should be private??
target_include_directories(${PROJECT_NAME}
        ${PUBLIC_ACCESS}
		${fmt_INCLUDE_DIRS}
		${spdlog_INCLUDE_DIRS}
        #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bignato/include>
)


#was explicit
target_precompile_headers(${PROJECT_NAME}
        ${PRIVATE_ACCESS}
        src/Lexicon/PCH.h)

#rogues gallery stuff
set(RoguePath "C:/Users/Noah/Desktop/Modding/[Lab]/{Lab Tools}/RoguesGallery")


add_subdirectory(${RoguePath} RoguesGallery)

add_dependencies(${PROJECT_NAME} RoguesGallery::RoguesGallery)

#was explicit
target_include_directories(
	${PROJECT_NAME}
	${PUBLIC_ACCESS}
		${RoguePath}/include
)



#target_link_libraries(
#	${PROJECT_NAME}
#	PUBLIC
#		RoguesGallery::RoguesGallery
#)


#Maybe I gotta include?
#include(CTest)
#include(Catch)

#temporary, for build tests only
#add_executable(${PROJECT_NAME}_Test src/main.cpp
#            ${headers})

#also interface
#Also, should this link it to the CMAKE project name?
target_link_libraries(
	${PROJECT_NAME}
	${PRIVATE_ACCESS}
		#RoguesGallery::RoguesGallery
		fmt::fmt
		spdlog::spdlog
		#tomlplusplus::tomlplusplus
		#Boost::boost_regex
)

add_compile_options(/Zc:preprocessor /permissive-)
target_compile_options(
		${PROJECT_NAME}
		${PUBLIC_ACCESS}
			#/sdl	# Enable Additional Security Checks
			#/utf-8	# Set Source and Executable character sets to UTF-8
			#/Zi	# Debug Information Format
			/permissive-	# Standards conformance

			#/Zc:alignedNew	# C++17 over-aligned allocation
			#/Zc:auto	# Deduce Variable Type
			#/Zc:char8_t
			#/Zc:__cplusplus	# Enable updated __cplusplus macro
			#/Zc:externC
			#/Zc:externConstexpr	# Enable extern constexpr variables
			#/Zc:forScope	# Force Conformance in for Loop Scope
			#/Zc:hiddenFriend
			#/Zc:implicitNoexcept	# Implicit Exception Specifiers
			#/Zc:lambda
			#/Zc:noexceptTypes	# C++17 noexcept rules
			/Zc:preprocessor	# Enable preprocessor conformance mode
			#/Zc:referenceBinding	# Enforce reference binding rules
			#/Zc:rvalueCast	# Enforce type conversion rules
			#/Zc:sizedDealloc	# Enable Global Sized Deallocation Functions
			#/Zc:strictStrings	# Disable string literal type conversion
			#/Zc:ternary	# Enforce conditional operator rules
			#/Zc:threadSafeInit	# Thread-safe Local Static Initialization
			#/Zc:tlsGuards
			#/Zc:trigraphs	# Trigraphs Substitution
			#/Zc:wchar_t	# wchar_t Is Native Type

			#/external:anglebrackets
			#/external:W0

			#/W4	# Warning level
			##/WX	# Warning level (warnings are errors)

			#"$<$<CONFIG:DEBUG>:>"
			#"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

# ---- Create an installable target ----

install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${PROJECT_NAME}-targets
)


install(
	DIRECTORY include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	DIRECTORY bignato/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(
	cmake/config.cmake.in
	${PROJECT_NAME}Config.cmake
	@ONLY
)



install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
	EXPORT ${PROJECT_NAME}-targets
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)




#///Executable Test
#add_executable (${PROJECT_NAME}_Test ${sources})
	add_executable (${PROJECT_NAME}_Test 
	"include/Lexicon/Impl/TempConstruct.cpp"
	"src/main.cpp")

	#for the includes of this project? For when #include is being used?
	target_include_directories(
		"${PROJECT_NAME}_Test"
		PUBLIC
			"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
			"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
	)

	#the includes of the projects that Im using as well
	target_include_directories(
		"${PROJECT_NAME}_Test"
		PRIVATE
			#${Catch2_INCLUDE_DIRS}
			${fmt_INCLUDE_DIRS}
			${spdlog_INCLUDE_DIRS}
			#${RAPIDCSV_INCLUDE_DIRS}
	)

	#Unsure what does, will not use.
	#target_compile_definitions(
	#	"${PROJECT_NAME}Tests"
	#	PUBLIC
	#		ENABLE_COMMONLIBSSE_TESTING=1
	#)

	#links the library made for the executable I guess?
	target_link_libraries(
		"${PROJECT_NAME}_Test"
		PRIVATE
			${PROJECT_NAME}
			fmt::fmt
			spdlog::spdlog
			#tomlplusplus::tomlplusplus
			##Catch2::Catch2
	)


	target_precompile_headers(
		"${PROJECT_NAME}_Test"
		PRIVATE
			src/Lexicon/PCH.h
	)
#//#

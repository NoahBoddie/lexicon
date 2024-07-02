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


#TODO: use this and a build setting to determine if this is trying to make a source plugin or if it's an interface
message("Options:")
option(LEXICON_SOURCE "Marks project as source binary for Lexicon." OFF)


# ---- Dependencies ----

find_package(fmt REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
#find_package(tomlplusplus REQUIRED CONFIG)
#find_package(Boost MODULE REQUIRED)

# ---- Add source files ----

set(headers 
#include/Lexicon.h
#
#include/Lexicon/AbstractFunction.h
#include/Lexicon/AbstractTypePolicy.h
#include/Lexicon/AccessModifier.h
#include/Lexicon/Array.h
#include/Lexicon/Component.h
#include/Lexicon/DataType.h
#include/Lexicon/DeclareSpecifier.h
#include/Lexicon/Default.h
#include/Lexicon/Dispatcher.h
#include/Lexicon/Element.h
#include/Lexicon/Environment.h
#include/Lexicon/Exception.h
#include/Lexicon/Field.h
#include/Lexicon/ICallableUnit.h
#include/Lexicon/IFunction.h
#include/Lexicon/IGenericArgument.h
#include/Lexicon/IGenericParameter.h
#include/Lexicon/ISpecial.h
#include/Lexicon/ITypePolicy.h
#include/Lexicon/IVariable.h
#include/Lexicon/MemberPointer.h
#include/Lexicon/Number.h
#include/Lexicon/Object.h
#include/Lexicon/ObjectData.h
#include/Lexicon/ObjectInfo.h
#include/Lexicon/ObjectPolicy.h
#include/Lexicon/ObjectPolicyHandle.h
#include/Lexicon/ObjectPolicyManager.h
#include/Lexicon/ObjectPoolData.h
#include/Lexicon/QualifiedField.h
#include/Lexicon/QualifiedType.h
#include/Lexicon/Qualifier.h
#include/Lexicon/Register.h
#include/Lexicon/Report.h
#include/Lexicon/Runtime.h
#include/Lexicon/RuntimeVariable.h
#include/Lexicon/Script.h
#include/Lexicon/String.h
#include/Lexicon/TestField.h
#include/Lexicon/TypeAliases.h
#include/Lexicon/TypeID.h
#include/Lexicon/Unvariable.h
#include/Lexicon/Variable.h
#include/Lexicon/VariableType.h
#include/Lexicon/VersionDeclareSpace.h
#include/Lexicon/Versioning.h
#
#include/Lexicon/Impl/IdentityManager.h
#include/Lexicon/Impl/Interface.h
#include/Lexicon/Impl/InterfaceManager.h
#include/Lexicon/Impl/InterfaceSingleton.h
##include/Lexicon/Impl/PCH.h
#include/Lexicon/Impl/ProcedureHandler.h
#include/Lexicon/Impl/ProjectManager.h
#
#	src/Lexicon/Impl/Implementation.h
#	src/Lexicon/Impl/Native/CompileUtility.h
#	src/Lexicon/Impl/Native/ConcreteFunction.h
#	src/Lexicon/Impl/Native/ConcretePolicy.h
#	src/Lexicon/Impl/Native/Conversion.h
#	src/Lexicon/Impl/Native/Declaration.h
#	src/Lexicon/Impl/Native/Expression.h
#	src/Lexicon/Impl/Native/ExpressionType.h
#	src/Lexicon/Impl/Native/FunctionBase.h
#	src/Lexicon/Impl/Native/FunctionData.h
#	src/Lexicon/Impl/Native/FunctionInfo.h
#	src/Lexicon/Impl/Native/GlobalData.h
#	src/Lexicon/Impl/Native/GlobalVariable.h
#	src/Lexicon/Impl/Native/HeaderSettings.h
#	src/Lexicon/Impl/Native/HierarchyData.h
#	src/Lexicon/Impl/Native/InfoBase.h
#	src/Lexicon/Impl/Native/InheritData.h
#	src/Lexicon/Impl/Native/InputStream.h
#	src/Lexicon/Impl/Native/Instruction.h
#	src/Lexicon/Impl/Native/InstructionType.h
#	src/Lexicon/Impl/Native/Literal.h
#	src/Lexicon/Impl/Native/LiteralManager.h
#	src/Lexicon/Impl/Native/LocalInfo.h
#	src/Lexicon/Impl/Native/MemberInfo.h
#	src/Lexicon/Impl/Native/Operand.h
#	src/Lexicon/Impl/Native/OperandType.h
#	src/Lexicon/Impl/Native/Operation.h
#	src/Lexicon/Impl/Native/OperatorType.h
#	src/Lexicon/Impl/Native/Overload.h
#	src/Lexicon/Impl/Native/OverloadClause.h
#	src/Lexicon/Impl/Native/OverloadEntry.h
#	src/Lexicon/Impl/Native/OverloadFlag.h
#	src/Lexicon/Impl/Native/OverloadInput.h
#	src/Lexicon/Impl/Native/OverloadKey.h
#	src/Lexicon/Impl/Native/ParameterInfo.h
#	src/Lexicon/Impl/Native/parse_strings.h
#	src/Lexicon/Impl/Native/ParseHandler.h
#	src/Lexicon/Impl/Native/ParseModule.h
#	src/Lexicon/Impl/Native/Parser.h
#	src/Lexicon/Impl/Native/ParserTest.h
#	src/Lexicon/Impl/Native/PolicyBase.h
#	src/Lexicon/Impl/Native/PolicyData.h
#	src/Lexicon/Impl/Native/ProcessContext.h
#	src/Lexicon/Impl/Native/Project.h
#	src/Lexicon/Impl/Native/RoutineBase.h
#	src/Lexicon/Impl/Native/RoutineCompiler.h
#	src/Lexicon/Impl/Native/Scope.h
#	src/Lexicon/Impl/Native/Signature.h
#	src/Lexicon/Impl/Native/Solution.h
#	src/Lexicon/Impl/Native/Target.h
#	src/Lexicon/Impl/Native/TargetObject.h
#	src/Lexicon/Impl/Native/Token.h
#	src/Lexicon/Impl/Native/TokenHandler.h
#	src/Lexicon/Impl/Native/TokenStream.h
#	src/Lexicon/Impl/Native/TokenType.h
#	src/Lexicon/Impl/Native/VariableInfo.h
#	src/Lexicon/Impl/Native/VoidPolicy.h
)

set(sources
#src/Lexicon/AbstractTypePolicy.cpp
#src/Lexicon/Component.cpp
#src/Lexicon/Dispatcher.cpp
#src/Lexicon/Element.cpp
#src/Lexicon/Environment.cpp
#src/Lexicon/Field.cpp
#src/Lexicon/ICallableUnit.cpp
#src/Lexicon/ITypePolicy.cpp
#src/Lexicon/ObjectPolicy.cpp
#src/Lexicon/ObjectPolicyHandle.cpp
#src/Lexicon/ObjectPolicyManager.cpp
#src/Lexicon/QualifiedType.cpp
#src/Lexicon/Script.cpp
#src/Lexicon/TypeID.cpp
#src/Lexicon/Variable.cpp
#src/Lexicon/VariableType.cpp
#src/Lexicon/Project.cpp
#src/Lexicon/Impl/IdentityManager.cpp
#	src/Lexicon/Impl/InterfaceManager.cpp
#	src/Lexicon/Impl/ProcedureHandler.cpp
#	src/Lexicon/Impl/ProjectManager.cpp
#	src/Lexicon/Impl/Native/ConcreteFunction.cpp
#	src/Lexicon/Impl/Native/ConcretePolicy.cpp
#	src/Lexicon/Impl/Native/Declaration.cpp
#	src/Lexicon/Impl/Native/FunctionBase.cpp
#	src/Lexicon/Impl/Native/HierarchyData.cpp
#	src/Lexicon/Impl/Native/InputStream.cpp
#	src/Lexicon/Impl/Native/Literal.cpp
#	src/Lexicon/Impl/Native/LiteralManager.cpp
#	src/Lexicon/Impl/Native/Operand.cpp
#	src/Lexicon/Impl/Native/Operation.cpp
#	src/Lexicon/Impl/Native/OverloadEntry.cpp
#	src/Lexicon/Impl/Native/OverloadInput.cpp
#	src/Lexicon/Impl/Native/OverloadKey.cpp
#	src/Lexicon/Impl/Native/ParseHandler.cpp
#	src/Lexicon/Impl/Native/ParseModule.cpp
#	src/Lexicon/Impl/Native/Parser.cpp
#	src/Lexicon/Impl/Native/PolicyBase.cpp
#	src/Lexicon/Impl/Native/ProcessContext.cpp
#	src/Lexicon/Impl/Native/RoutineCompiler.cpp
#	src/Lexicon/Impl/Native/Scope.cpp
#	src/Lexicon/Impl/Native/TempConstruct.cpp
#	src/Lexicon/Impl/Native/TokenHandler.cpp
#	src/Lexicon/Impl/Native/TokenStream.cpp
#	src/Lexicon/Impl/Native/VariableInfo.cpp
#	src/Lexicon/Impl/Native/VoidPolicy.cpp
)





source_group(
	TREE
		${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${headers}
		${sources}
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

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(
	${PROJECT_NAME}
	${PUBLIC_ACCESS}
		cxx_std_23
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" 
					"${CMAKE_CURRENT_SOURCE_DIR}/BigNato/include" 
#					"${CMAKE_CURRENT_SOURCE_DIR}/include/Lexicon"
#					"${CMAKE_CURRENT_SOURCE_DIR}/include/Lexicon/Impl"
) 





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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)


#was explicit
target_precompile_headers(${PROJECT_NAME}
        ${PRIVATE_ACCESS}
        include/Lexicon/Impl/PCH.h)

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
	#"src/Lexicon/Impl/Native/TempConstruct.cpp"
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
			include/Lexicon/Impl/PCH.h
	)
#//#

cmake_minimum_required(VERSION 3.21)

# ---- Project ----

set(PARENT_NAME "${PROJECT_NAME}")

project(
	Lexicon
	LANGUAGES CXX
)

if (NOT PARENT_NAME)
	set(PARENT_NAME "${PROJECT_NAME}")
endif()




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
#message("Options:")
#option(LEXICON_SOURCE "Marks project as source binary for Lexicon." ON)
#option(TEST_BUILD "Tries to use test build for lexicon." ON)#if project is self, allow test builds.

if (PARENT_NAME STREQUAL PROJECT_NAME)
	set(TEST_BUILD ON)
endif()

if (CMAKE_PROJECT_NAME STREQUAL PARENT_NAME)
	message("* LEXICON_SOURCE = ON")
    set(LEXICON_SOURCE ON)
endif()


# ---- Dependencies ----

find_package(fmt REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
#find_package(tomlplusplus REQUIRED CONFIG)
#find_package(Boost MODULE REQUIRED)

# ---- Add source files ----




if(LEXICON_SOURCE)
	set(master_headers
		

		native/include/Lexicon/Engine.h
		native/include/Lexicon/Engine/Impl/EngineEnum.h
		
		native/include/Lexicon/Engine/TempConstruct.h
		"native/include/Lexicon/Engine/TestField.h"
		"native/include/Lexicon/Engine/TestFieldB.h"
		"native/include/Lexicon/Engine/QualifiedField.h"
		native/include/Lexicon/Engine/CompileUtility.h
		native/include/Lexicon/Engine/ConcreteGlobal.h
		native/include/Lexicon/Engine/ConcreteFunction.h
		native/include/Lexicon/Engine/ConcreteType.h
		native/include/Lexicon/Engine/Declaration.h
		native/include/Lexicon/Engine/Expression.h
		native/include/Lexicon/Engine/ExpressionType.h
		native/include/Lexicon/Engine/FunctionBase.h
		native/include/Lexicon/Engine/GlobalBase.h
		native/include/Lexicon/Engine/FunctionData.h
		native/include/Lexicon/Engine/FunctionInfo.h
		native/include/Lexicon/Engine/GlobalData.h
		native/include/Lexicon/Engine/GlobalVariable.h
		native/include/Lexicon/Engine/HeaderSettings.h
		native/include/Lexicon/Engine/HierarchyData.h
		native/include/Lexicon/Engine/InfoBase.h
		native/include/Lexicon/Engine/InheritData.h
		native/include/Lexicon/Engine/InputStream.h
		native/include/Lexicon/Engine/Instruction.h
		native/include/Lexicon/Engine/InstructionType.h
		native/include/Lexicon/Engine/Literal.h
		native/include/Lexicon/Engine/LiteralManager.h
		native/include/Lexicon/Engine/LocalInfo.h
		native/include/Lexicon/Engine/MemberInfo.h
		native/include/Lexicon/Engine/Operand.h
		native/include/Lexicon/Engine/OperandType.h
		native/include/Lexicon/Engine/Operation.h
		native/include/Lexicon/Engine/OperatorType.h
		native/include/Lexicon/Engine/Overload.h
		native/include/Lexicon/Engine/OverloadClause.h
		native/include/Lexicon/Engine/OverloadEntry.h
		native/include/Lexicon/Engine/OverloadFlag.h
		native/include/Lexicon/Engine/OverloadInput.h
		native/include/Lexicon/Engine/OverloadKey.h
		native/include/Lexicon/Engine/ParameterInfo.h
		native/include/Lexicon/Engine/parse_strings.h
		native/include/Lexicon/Engine/ParseHandler.h
		native/include/Lexicon/Engine/ParseModule.h
		native/include/Lexicon/Engine/Parser.h
		native/include/Lexicon/Engine/ParserTest.h
		native/include/Lexicon/Engine/TypeBase.h
		native/include/Lexicon/Engine/PolicyData.h
		native/include/Lexicon/Engine/ProcessContext.h
		native/include/Lexicon/Engine/Register.h
		native/include/Lexicon/Engine/RoutineBase.h
		native/include/Lexicon/Engine/RoutineCompiler.h
		native/include/Lexicon/Engine/Scope.h
		native/include/Lexicon/Engine/Signature.h
		native/include/Lexicon/Engine/Solution.h
		native/include/Lexicon/Engine/Target.h
		native/include/Lexicon/Engine/TargetObject.h
		native/include/Lexicon/Engine/Token.h
		native/include/Lexicon/Engine/TokenHandler.h
		native/include/Lexicon/Engine/TokenStream.h
		native/include/Lexicon/Engine/TokenType.h
		native/include/Lexicon/Engine/VariableInfo.h
		native/include/Lexicon/Engine/VoidPolicy.h
		native/include/Lexicon/Engine/GlobalInfo.h
		"native/include/Lexicon/Engine/Element.h"
		"native/include/Lexicon/Engine/Environment.h"
		"native/include/Lexicon/Engine/Project.h"
		"native/include/Lexicon/Engine/Script.h"
		"native/include/Lexicon/Engine/SpecialBase.h"
		"native/include/Lexicon/Engine/GenericType.h" 
		"native/include/Lexicon/Engine/DynamicTupleElement.h" 
		"native/include/Lexicon/Engine/TemplateTuple.h" 
		"native/include/Lexicon/Engine/Component.h"
		"native/include/Lexicon/Engine/SettingManager.h"
		"native/include/Lexicon/Engine/DefaultClient.h" 
		"native/include/Lexicon/Engine/SyntaxRecord.h" 
		"native/include/Lexicon/Engine/BasicFormula.h"
		"native/include/Lexicon/Engine/Runtime.h"
		native/include/Lexicon/Engine/OperatorSetting.h
		native/include/Lexicon/Engine/Impl/FunctionRegister.h
		native/include/Lexicon/Engine/IElementImpl.h
		native/include/Lexicon/Engine/Impl/IElementHeader.h
		native/include/Lexicon/Engine/Impl/IElementSource.h
		"native/include/Lexicon/Engine/IssueTable.h"
		"native/include/Lexicon/Engine/SpecialType.h"
		"native/include/Lexicon/Engine/AbstractType.h"
	)
		

	set(master_sources
	
		"native/src/Lexicon/Engine/QualifiedField.cpp"
		native/src/Lexicon/Engine/CompileUtility.cpp
		native/src/Lexicon/Engine/GenericBase.cpp
		native/src/Lexicon/Engine/ConcreteFunction.cpp
		native/src/Lexicon/Engine/ConcreteType.cpp
		native/src/Lexicon/Engine/Declaration.cpp
		native/src/Lexicon/Engine/FunctionBase.cpp
		native/src/Lexicon/Engine/HierarchyData.cpp
		native/src/Lexicon/Engine/InputStream.cpp
		native/src/Lexicon/Engine/Literal.cpp
		native/src/Lexicon/Engine/LiteralManager.cpp
		native/src/Lexicon/Engine/Operand.cpp
		native/src/Lexicon/Engine/Operation.cpp
		native/src/Lexicon/Engine/OverloadEntry.cpp
		native/src/Lexicon/Engine/OverloadInput.cpp
		native/src/Lexicon/Engine/OverloadKey.cpp
		native/src/Lexicon/Engine/ParseHandler.cpp
		native/src/Lexicon/Engine/ParseModule.cpp
		native/src/Lexicon/Engine/Parser.cpp
		native/src/Lexicon/Engine/TypeBase.cpp
		native/src/Lexicon/Engine/ProcessContext.cpp
		native/src/Lexicon/Engine/RoutineCompiler.cpp
		native/src/Lexicon/Engine/Scope.cpp
		native/src/Lexicon/Engine/TempConstruct.cpp
		native/src/Lexicon/Engine/TokenHandler.cpp
		native/src/Lexicon/Engine/TokenStream.cpp
		native/src/Lexicon/Engine/VariableInfo.cpp
		native/src/Lexicon/Engine/VoidPolicy.cpp
		native/src/Lexicon/Engine/InfoBase.cpp
		native/src/Lexicon/Engine/GlobalBase.cpp
		native/src/Lexicon/Engine/Script.cpp
		native/src/Lexicon/Engine/Element.cpp
		native/src/Lexicon/Engine/Environment.cpp	
		src/Lexicon/Interfaces/ObjectPolicyManager.cpp
		"native/src/Lexicon/Engine/Component.cpp"
		native/src/Lexicon/Engine/Project.cpp
		src/Lexicon/Interfaces/IdentityManager.cpp
		src/Lexicon/Interfaces/InterfaceManager.cpp
		src/Lexicon/Interfaces/ProcedureHandler.cpp
		src/Lexicon/Interfaces/ProjectManager.cpp
		src/Lexicon/Interfaces/LinkMessenger.cpp
		"native/src/Lexicon/Engine/GenericType.cpp"
		"native/src/Lexicon/Engine/TemplateType.cpp" 
		src/Lexicon/Interfaces/FormulaManager.cpp
		"src/Lexicon/Interfaces/ReportManager.cpp"
		"native/src/Lexicon/Engine/SettingManager.cpp"
		"native/src/Lexicon/Engine/FunctionData.cpp" 
		"native/src/Lexicon/Engine/SyntaxRecord.cpp"
		"native/src/Lexicon/Engine/IElementImpl.cpp"
		"native/src/Lexicon/Engine/IProjectImpl.cpp" 
		"native/src/Lexicon/Engine/IScriptImpl.cpp"
		"native/src/Lexicon/Engine/TargetObject.cpp"
		"native/src/Lexicon/Engine/SpecialBase.cpp"
		)
else()
	set (master_headers )
	set (master_sources )
endif()

set(headers
	include/Lexicon.h
	include/Lexicon/AbstractGlobal.h
	include/Lexicon/Formula.h
	include/Lexicon/AbstractFunction.h
	include/Lexicon/Type.h
	include/Lexicon/AccessModifier.h
	include/Lexicon/Array.h
	include/Lexicon/Conversion.h
	include/Lexicon/DataType.h
	include/Lexicon/DeclareSpecifier.h
	include/Lexicon/Default.h
	include/Lexicon/Dispatcher.h
	include/Lexicon/Exception.h
	include/Lexicon/Field.h
	include/Lexicon/ICallableUnit.h
	include/Lexicon/IFunction.h
	
	
	include/Lexicon/ISpecial.h
	include/Lexicon/IType.h
	include/Lexicon/IGlobal.h
	include/Lexicon/IVariable.h
	include/Lexicon/MemberPointer.h
	include/Lexicon/Number.h
	include/Lexicon/Object.h
	include/Lexicon/ObjectData.h
	include/Lexicon/ObjectInfo.h
	include/Lexicon/ObjectPolicy.h
	include/Lexicon/ObjectPolicyHandle.h
	"include/Lexicon/Interfaces/ObjectPolicyManager.h"
	include/Lexicon/ObjectPoolData.h
	include/Lexicon/QualifiedType.h
	include/Lexicon/Qualifier.h
	include/Lexicon/Report.h
	include/Lexicon/RuntimeVariable.h
	include/Lexicon/String.h
	include/Lexicon/TypeAliases.h
	include/Lexicon/TypeID.h
	include/Lexicon/Unvariable.h
	include/Lexicon/Variable.h
	include/Lexicon/VariableType.h
	include/Lexicon/VersionDeclareSpace.h
	include/Lexicon/Versioning.h
	include/Lexicon/Impl/PCH.h
	include/Lexicon/Impl/ref_wrapper.h
	include/Lexicon/Interfaces/IdentityManager.h
	include/Lexicon/Interfaces/Interface.h
	"include/Lexicon/Interfaces/InterfaceManager.h"
	include/Lexicon/Interfaces/InterfaceSingleton.h
	include/Lexicon/Interfaces/ProcedureHandler.h
	include/Lexicon/Interfaces/ProjectManager.h
	"include/Lexicon/Interfaces/IElement.h" 
	"include/Lexicon/Interfaces/IEnvironment.h"
	"include/Lexicon/Interfaces/IScript.h" 
	"include/Lexicon/Interfaces/IProject.h"
	"include/Lexicon/Interfaces/ProjectClient.h"
	"include/Lexicon/SignatureBase.h" 
	"include/Lexicon/IRuntime.h"
	"include/Lexicon/ProcedureData.h" 
	"include/Lexicon/InherentType.h"
	"include/Lexicon/ElementType.h" 
	"include/Lexicon/RelateType.h"
	"include/Lexicon/IFormula.h"
	"include/Lexicon/Interfaces/FormulaManager.h"
	"include/Lexicon/FormulaHandler.h"
	"include/Lexicon/Interfaces/ReportManager.h" 
	"include/Lexicon/Impl/common_type.h" 
	
	 
	 
	
	"include/Lexicon/IElementProxy.h"
	"include/Lexicon/IElementProxy.h"
	"include/Lexicon/Issue.h"
	"include/Lexicon/Interfaces/LinkMessenger.h"
	${master_headers}
  "include/Lexicon/IssueCode.h" "include/Lexicon/IniHandler.h"   "include/Lexicon/LinkFlag.h" "native/include/Lexicon/Engine/OperateList.h" "include/Lexicon/NativeReference.h" "native/include/Lexicon/Engine/RuntimeUtility.h" "include/Lexicon/ITemplateBodyPart.h" "include/Lexicon/GenericArray.h")


set(sources
	src/Lexicon/Type.cpp
	src/Lexicon/Dispatcher.cpp

	src/Lexicon/Field.cpp
	src/Lexicon/ICallableUnit.cpp
	src/Lexicon/IType.cpp
	src/Lexicon/ObjectPolicy.cpp
	src/Lexicon/ObjectPolicyHandle.cpp
	src/Lexicon/QualifiedType.cpp
	src/Lexicon/TypeID.cpp
	src/Lexicon/Variable.cpp
	src/Lexicon/VariableType.cpp
	src/Lexicon/ITemplatePart.cpp

	"src/Lexicon/ObjectInfo.cpp"
	"include/Lexicon/Report.cpp"#this is in the wrong place.

	"src/Lexicon/FormulaHandler.cpp"
	"src/Lexicon/Object.cpp"
	"src/Lexicon/Impl/PCH.cpp"
	${master_sources}
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

if(LEXICON_SOURCE)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" 
					"${CMAKE_CURRENT_SOURCE_DIR}/native/include" 
#					"${CMAKE_CURRENT_SOURCE_DIR}/include/Lexicon"
#					"${CMAKE_CURRENT_SOURCE_DIR}/include/Lexicon/Interfaces"
) 





#I moved this in myself
target_include_directories(${PROJECT_NAME}
        ${PRIVATE_ACCESS}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/native/src>
        $<INSTALL_INTERFACE:src>)

#interface


#was explicitly public, should be private??
target_include_directories(${PROJECT_NAME}
        ${PUBLIC_ACCESS}
		${fmt_INCLUDE_DIRS}
		${spdlog_INCLUDE_DIRS}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/native/include>
)

else()
	
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include"
	) 





	#I moved this in myself
	target_include_directories(${PROJECT_NAME}
			${PRIVATE_ACCESS}
			$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
			$<INSTALL_INTERFACE:src>)

	#interface


	#was explicitly public, should be private??
	target_include_directories(${PROJECT_NAME}
			${PUBLIC_ACCESS}
			${fmt_INCLUDE_DIRS}
			${spdlog_INCLUDE_DIRS}
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	)
endif()


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
if(LEXICON_SOURCE)

	install(
		DIRECTORY native/include/
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)
endif()

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




get_target_property(BINARY_TYPE ${PARENT_NAME} TYPE)
message(${BINARY_TYPE})
if (${BINARY_TYPE} STREQUAL EXECUTABLE)
	set(BINARY_TYPE exe)
elseif (${BINARY_TYPE} STREQUAL SHARED_LIBRARY)
	set(BINARY_TYPE dll)
else()
	#This is an error option, lib should never find its way into this.
	# IE, if there's no test build version, this shit has to find its way out of here.
	set(BINARY_TYPE lib)
endif()

if (CMAKE_PROJECT_NAME STREQUAL PARENT_NAME)
    target_compile_definitions(
	"${PROJECT_NAME}"
	PUBLIC
		LEX_SOURCE=1
	)

    #when this gets ported out, please use it to also determine what the primary source is as well
endif()

target_compile_definitions(
	"${PROJECT_NAME}"
	PUBLIC
		LEX_BINARY_MODULE=\"${PARENT_NAME}.${BINARY_TYPE}\"
	)


if (SETTING_PATH)

	target_compile_definitions(
		"${PROJECT_NAME}"
		PUBLIC
			SETTING_PATH="${SETTING_PATH}"
		)

	target_compile_definitions(
		"${PARENT_NAME}"
		PUBLIC
			SETTING_PATH="${SETTING_PATH}"
	)
endif()

#///Executable Test
#add_executable (${PROJECT_NAME}_Test ${sources})
if(LEXICON_SOURCE AND TEST_BUILD)
	add_executable (${PROJECT_NAME}_Test 
	native/src/Lexicon/Engine/TempConstruct.cpp
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

	#This should be specifically for test builds I guess.
	target_compile_definitions(
	"${PROJECT_NAME}"
	PUBLIC
		LEX_BINARY_MODULE=\"${PARENT_NAME}_Test.exe\"
	)

	

	target_compile_definitions(
	"${PROJECT_NAME}"
	PUBLIC
		LEX_TEST_SOURCE=1
	)
endif()



#get_target_property(CHECK_TYPE ${PARENT_NAME} TYPE)
#message(${CHECK_TYPE})
#if (${CHECK_TYPE} STREQUAL MODULE_LIBRARY)
#message("TestThing")
#endif()


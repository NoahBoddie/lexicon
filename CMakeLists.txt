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
find_package(tomlplusplus REQUIRED CONFIG)
#find_package(Boost MODULE REQUIRED)

# ---- Add source files ----

set(headers 
include/Lexicon.h
#include/Lexicon/Impl/LexComponent.h
#include/Lexicon/Impl/TOMLHandler.h
#include/Lexicon/Impl/Exception.h
include/Lexicon/Impl/Parser.h
include/Lexicon/Impl/ParseModule.h
)

set(sources
src/main.cpp
#include/Lexicon/Impl/ParseModule.h
src/Lexicon/Impl/Parser.cpp
src/Lexicon/Impl/ParseModule.cpp
src/Lexicon/Impl/ParseHandler.cpp
src/Lexicon/Impl/ProcessContext.cpp
src/Lexicon/Impl/InputStream.cpp
src/Lexicon/Impl/TokenStream.cpp
src/Lexicon/Component.cpp
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
		#PUBLIC
		${headers}
		${sources}
		.clang-format
	)
else()
	set(PUBLIC_ACCESS INTERFACE)
	set(PRIVATE_ACCESS INTERFACE)

	add_library(
		${PROJECT_NAME}
		INTERFACE
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

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include") 





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
		RoguesGallery::RoguesGallery
		fmt::fmt
		spdlog::spdlog
		tomlplusplus::tomlplusplus
		#Boost::boost_regex
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

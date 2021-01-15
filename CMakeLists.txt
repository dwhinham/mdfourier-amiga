# CMake build system for mdfourier-amiga
# Copyright (C) 2021 Dale Whinham <daleyo@gmail.com>

cmake_minimum_required(VERSION 3.13)
project(mdfourier ASM C)

# Look for Amiga-GCC
if(NOT AMIGA_GCC)
	if(DEFINED ENV{AMIGA_GCC})
		set(AMIGA_GCC $ENV{AMIGA_GCC} CACHE STRING "Path to Amiga-GCC.")
	else()
		message(FATAL_ERROR "Couldn't find Amiga-GCC. Please set the AMIGA_GCC environment variable.")
	endif()
endif()

# Look for required external tools
find_package(Git)

# If Git was found, add a custom target to generate version information for every build
if(GIT_FOUND)
	add_custom_target(version ALL
		COMMAND ${CMAKE_COMMAND} -DGIT_EXECUTABLE=${GIT_EXECUTABLE} -DAPP_NAME=${CMAKE_PROJECT_NAME} -P ${CMAKE_SOURCE_DIR}/cmake/version.cmake
		COMMENT "Generating version information using git"
		SOURCES src/version.c.in
	)

	# Prevent CMake from looking for this nonexistant file at configure time
	set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/version.c PROPERTIES GENERATED TRUE)
endif()

# Override release C flags (defaults to -O3)
set(CMAKE_C_FLAGS_DEBUG "-fomit-frame-pointer")
set(CMAKE_C_FLAGS_RELEASE "-Os -s -fomit-frame-pointer")

# Assembly target
add_library(vblank_wrapper OBJECT src/vblank_wrapper.s)
target_compile_options(vblank_wrapper PRIVATE -m68000 -Fhunk -x -quiet)

# Executable target
add_executable(mdfourier
	# Sources
	src/main.c
	src/vblank.c
	src/waveform.c

	# Headers
	include/vblank.h
	include/waveform.h
)

# If version information will be generated, add it to the sources list
if(GIT_FOUND)
	target_sources(mdfourier PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.c)
endif()

set_target_properties(mdfourier PROPERTIES C_STANDARD 99)

# Link options must have -mcrt=nix13, -mregparm to match compile options
target_compile_options(mdfourier PRIVATE -mcrt=nix13 -Wall -Werror -Wno-pointer-sign)
target_link_options(mdfourier PRIVATE -mcrt=nix13)

target_compile_definitions(mdfourier PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_link_libraries(mdfourier vblank_wrapper amiga $<$<CONFIG:Debug>:debug>)
target_include_directories(mdfourier PRIVATE include external/optparse)
set_property(TARGET mdfourier PROPERTY C_STANDARD 99)

# Ensure version info is generated before building
if(GIT_FOUND)
	add_dependencies(mdfourier version)
endif()

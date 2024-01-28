#*****************************************************************************#
#    Copyright (C) 2014 Aali132                                               #
#    Copyright (C) 2024 Julian Xhokaxhiu                                      #
#                                                                             #
#    This file is part of Palmer                                              #
#                                                                             #
#    Palmer is free software: you can redistribute it and/or modify           #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    Palmer is distributed in the hope that it will be useful,                #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(FATAL_ERROR "CMAKE_BUILD_TYPE must be set to continue building with cmake. \nExample: Add -DCMAKE_BUILD_TYPE=Release to your cmake command line.")
endif()
if (NOT DEFINED _EXE_VERSION OR NOT _EXE_VERSION)
  message(FATAL_ERROR "_EXE_VERSION must be set to continue building with cmake. \nExample: Add -D_EXE_VERSION=devel to your cmake command line.")
endif ()

set(VCPKG_DISABLE_COMPILER_TRACKING 1)
set(VCPKG_INSTALL_OPTIONS "--clean-after-build" "--x-use-aria2")
set(VCPKG_TARGET_TRIPLET x86-windows-static)

# Currently missing by default, port bug?
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/share/freeglut")

set(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD /DEBUG:FULL /FORCE:MULTIPLE /IGNORE:4006,4075,4099,4217"
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
else()
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
endif()

project(Palmer)

find_package(FreeGLUT CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(libpng CONFIG REQUIRED)
find_package(mimalloc CONFIG REQUIRED)

set(RELEASE_NAME "Palmer")

if (_EXE_VERSION STREQUAL "devel" OR _EXE_VERSION MATCHES "-" )
    set(_EXE_RCVERSION "0,0,0,0")
    set(_EXE_RCSTRVERSION "0.0.0.0")
else()
    string(REPLACE "." "," _EXE_RCVERSION ${_EXE_VERSION})
    set(_EXE_RCSTRVERSION ${_EXE_VERSION})
endif()

# Include all the source code files
file(GLOB_RECURSE source_files "${CMAKE_SOURCE_DIR}/src/*.cpp")

configure_file(misc/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

add_executable(${RELEASE_NAME} WIN32 ${source_files} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
target_include_directories(
  ${RELEASE_NAME}
  PRIVATE "${CMAKE_SOURCE_DIR}/src"
)
target_link_libraries(
	${RELEASE_NAME}
	dbghelp
  mimalloc-static
  png_static
	FreeGLUT::freeglut_static
	GLEW::glew_s
)
target_compile_options(
	${RELEASE_NAME}
	PRIVATE /DVERSION="${_EXE_VERSION}"
	PRIVATE /D_CRT_SECURE_NO_WARNINGS
  PRIVATE /DNOMINMAX
  PRIVATE /Zc:strictStrings-
  PRIVATE /Zc:__cplusplus
  PRIVATE /Qpar
)
target_link_options(${RELEASE_NAME} PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb)

# INSTALL
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
  # License
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt
          ${CMAKE_BINARY_DIR}/bin
  # .exe
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RELEASE_NAME}.exe
          ${CMAKE_BINARY_DIR}/bin
)

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
add_custom_command(
  TARGET ${RELEASE_NAME}
  POST_BUILD
  # ensure bin directory exists
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
  # .pdb
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RELEASE_NAME}.pdb
          ${CMAKE_BINARY_DIR}/bin
)
endif()

# CPU INFO
add_custom_command(
  TARGET ${RELEASE_NAME}
  PRE_BUILD
  # Get CPU info
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/cpuinfo/cpu-info
  # Get CPU ISA supported instructions
  COMMAND
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows-static/tools/cpuinfo/isa-info
)

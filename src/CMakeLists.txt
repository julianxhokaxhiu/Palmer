#*****************************************************************************#
#    Copyright (C) 2014 Aali132                                               #
#    Copyright (C) 2020 Julian Xhokaxhiu                                      #
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

find_package(FreeGLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(LIBPNG REQUIRED)

set(RELEASE_NAME "Palmer")

if (_EXE_VERSION STREQUAL "devel" OR _EXE_VERSION MATCHES "-" )
    set(_EXE_RCVERSION "0,0,0,0")
    set(_EXE_RCSTRVERSION "0.0.0.0")
else()
    string(REPLACE "." "," _EXE_RCVERSION ${_EXE_VERSION})
    set(_EXE_RCSTRVERSION ${_EXE_VERSION})
endif()

# Include all the source code files
file(GLOB_RECURSE source_files "*.cpp")

configure_file(
  ../misc/version.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/version.rc
  @ONLY
)

add_executable(${RELEASE_NAME} WIN32 ${source_files} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
target_include_directories(${RELEASE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${RELEASE_NAME}
    dbghelp
    FreeGLUT::freeglut_static
    GLEW::GLEW
    LIBPNG::LIBPNG
)
target_compile_options(${RELEASE_NAME}
    PRIVATE /DVERSION="${_EXE_VERSION}"
	PRIVATE /D_CRT_SECURE_NO_WARNINGS
	PRIVATE /DNOMINMAX
	PRIVATE /EHsc
    PRIVATE /std:c++latest
    PRIVATE /w
    PRIVATE /Ox
)
target_link_options(${RELEASE_NAME}
    PRIVATE /PDBALTPATH:${RELEASE_NAME}.pdb
)

# INSTALL
install(TARGETS ${RELEASE_NAME} RUNTIME DESTINATION bin)
install(FILES ../COPYING.TXT DESTINATION bin OPTIONAL)
install(FILES $<TARGET_PDB_FILE:${RELEASE_NAME}> DESTINATION bin OPTIONAL)

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

include(FindPackageHandleStandardArgs)

if (NOT ZLIB_FOUND)
	find_library(
		ZLIB_LIBRARY
		zlibd zlib
		PATH_SUFFIXES
		lib
	)

	find_path(
		ZLIB_INCLUDE_DIR
		zlib.h
		PATH_SUFFIXES
		include
	)

	add_library(ZLib::ZLib STATIC IMPORTED)

	set_target_properties(
		ZLib::ZLib
		PROPERTIES
		IMPORTED_LOCATION
		"${ZLIB_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES
		"${ZLIB_INCLUDE_DIR}"
	)

	find_package_handle_standard_args(ZLib DEFAULT_MSG ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
endif()

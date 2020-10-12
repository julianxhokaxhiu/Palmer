/****************************************************************************/
//    Copyright (C) 2014 Aali132                                            //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//                                                                          //
//    This file is part of Palmer                                           //
//                                                                          //
//    Palmer is free software: you can redistribute it and/or modify        //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    Palmer is distributed in the hope that it will be useful,             //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#include <string.h>

#include "palmer.h"

void section_read_bytes(uint32_t *iterator, void *dest, uint32_t bytes)
{
	memcpy(dest, (void *)*iterator, bytes);
	*iterator += bytes;
}

uint8_t section_read_byte(uint32_t *iterator)
{
	uint8_t ret;

	section_read_bytes(iterator, &ret, sizeof(ret));

	return ret;
}

uint16_t section_read_word(uint32_t *iterator)
{
	uint16_t ret;

	section_read_bytes(iterator, &ret, sizeof(ret));

	return ret;
}

uint32_t section_read_dword(uint32_t *iterator)
{
	uint32_t ret;

	section_read_bytes(iterator, &ret, sizeof(ret));

	return ret;
}

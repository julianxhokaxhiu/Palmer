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

unsigned char section_read_byte(uint *iterator)
{
	return *((unsigned char *)*iterator)++;
}

word section_read_word(uint *iterator)
{
	return *((word *)*iterator)++;
}

uint section_read_dword(uint *iterator)
{
	return *((uint *)*iterator)++;
}

void section_read_bytes(uint *iterator, void *dest, uint bytes)
{
	memcpy(dest, (void *)*iterator, bytes);
	*iterator += bytes;
}

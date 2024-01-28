/****************************************************************************/
//    Copyright (C) 2014 Aali132                                            //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include <windows.h>

#include "palmer.h"
#include "section.h"

#pragma pack(1)

struct section3_header
{
	uint32_t size;
	uint16_t unknown1;
	uint16_t unknown2;
	uint16_t palette_colors;
	uint16_t palettes;
};

void read_palettes(uint32_t iterator, uint32_t size)
{
	uint32_t i;
	uint32_t section_end = iterator + size;
	struct section3_header header;

	section_read_bytes(&iterator, &header, sizeof(header));

	state.palettes_num = header.palettes;
	state.palette_colors = header.palette_colors;

	for(i = 0; i < header.palettes; i++)
	{
		state.palettes[i] = (uint16_t *)iterator;
		iterator += header.palette_colors * 2;
	}

	if(iterator == section_end) state.palettes_ok = true;
	else
	{
		MessageBoxA(0, "Read past end of palette section.", "Error", MB_ICONERROR | MB_OK);
		state.palettes_ok = false;
	}
}

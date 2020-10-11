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

#include <windows.h>

#include "palmer.h"
#include "section.h"

#pragma pack(1)

struct section3_header
{
	uint size;
	word unknown1;
	word unknown2;
	word palette_colors;
	word palettes;
};

void read_palettes(uint iterator, uint size)
{
	uint i;
	uint section_end = iterator + size;
	struct section3_header header;

	section_read_bytes(&iterator, &header, sizeof(header));

	state.palettes_num = header.palettes;
	state.palette_colors = header.palette_colors;

	for(i = 0; i < header.palettes; i++)
	{
		state.palettes[i] = (word *)iterator;
		iterator += header.palette_colors * 2;
	}

	if(iterator == section_end) state.palettes_ok = true;
	else
	{
		MessageBoxA(0, "Read past end of palette section.", "Error", 0);
		state.palettes_ok = false;
	}
}

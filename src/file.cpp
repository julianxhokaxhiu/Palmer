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

#include <malloc.h>
#include <stdio.h>
#include <sys/stat.h>

#include <gl/freeglut.h>
#include <stdlib.h>

#include "palmer.h"
#include "section.h"
#include "layers.h"
#include "palettes.h"
#include "import.h"

void *old_buffer = 0;
uint32_t old_buffer_end;

void *malloc_read(FILE *f, int size)
{
	char *ret = (char*)malloc(size);
	char *data = ret;
	int res;

	do
	{
		res = fread(data, 1, size, f);
		if(res != 0)
		{
			size -= res;
			data += res;
		}
		else return ret;
	} while(size);

	return ret;
}

uint32_t unpack_lzs(uint32_t source, uint32_t dest, uint32_t real_source_end)
{
	uint32_t bit_counter; // [sp+14h] [bp-Ch]@1
	uint32_t source_end; // [sp+Ch] [bp-14h]@1
	uint32_t dest_start; // [sp+4h] [bp-1Ch]@1
	uint32_t block_byte; // [sp+18h] [bp-8h]@5
	uint32_t v7; // [sp+8h] [bp-18h]@12
	uint32_t v8; // [sp+10h] [bp-10h]@12
	uint32_t reference_out_end; // [sp+0h] [bp-20h]@12
	uint32_t reference_in; // [sp+1Ch] [bp-4h]@12

	bit_counter = 0;
	source_end = source + *(uint32_t *)source + 3;
	source += 4;
	dest_start = dest;

	if(source_end > real_source_end)
	{
		MessageBoxA(0, "LZS decompression would overflow, not doing that.", "Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	while(1)
	{
		if(!bit_counter)
		{
			bit_counter = 8;
			if(source >= source_end) return dest - dest_start;
			block_byte = *(uint8_t *)source++;
		}

		if(!(block_byte & 1)) break;

		if(source >= source_end) return dest - dest_start;

		*(uint8_t *)dest++ = *(uint8_t *)source++;

LABEL_17:
		block_byte >>= 1;
		--bit_counter;
	}

	if(source < (unsigned int)source_end)
	{
		v7 = *(uint8_t *)source++;
		v8 = *(uint8_t *)source++;
		v7 |= 16 * (uint8_t)(v8 & 0xF0);
		reference_out_end = dest + (v8 & 0xF) + 3;
		reference_in = dest - ((dest - dest_start - (v7 - 4078)) & 0xFFF);

		while(reference_in < dest_start)
		{
			*(uint8_t *)dest++ = 0;
			++reference_in;
		}

		while(dest < reference_out_end) *(uint8_t *)dest++ = *(uint8_t *)reference_in++;

		goto LABEL_17;
	}

	return dest - dest_start;
}

void parse_field_file(void *buffer, uint32_t buffer_end)
{
	struct field_header *header = (struct field_header *)buffer;
	uint32_t section3_iterator;
	uint32_t section8_iterator;
	uint32_t section3_size;
	uint32_t section8_size;
	uint32_t i;

	section3_iterator = header->section3 + (uint32_t)buffer;
	section8_iterator = header->section8 + (uint32_t)buffer;

	if(section3_iterator < (uint32_t)buffer || section3_iterator > buffer_end || section8_iterator < (uint32_t)buffer || section8_iterator > buffer_end)
	{
		MessageBoxA(0, "Bad sections, not going to read this file.", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	section3_size = section_read<uint32_t>(&section3_iterator);
	section8_size = section_read<uint32_t>(&section8_iterator);

	if(section3_iterator + section3_size - 4 > buffer_end || section8_iterator + section8_size - 4 > buffer_end)
	{
		MessageBoxA(0, "Bad sections, not going to read this file.", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	if(old_buffer) free(old_buffer);

	old_buffer = buffer;
	old_buffer_end = buffer_end;

	for(i = 0; i < EXPORT_LAYERS; i++)
	{
		if(state.export_layers[i]) free(state.export_layers[i]);
	}

	for(i = 0; i < FF7_LAYERS; i++)
	{
		glDeleteTextures(PALETTES, state.layers[i].textures);
		if(state.layers[i].imported_data) free(state.layers[i].imported_data);
	}

	memset(&state, 0, sizeof(state));
	state.scale = 1;

	read_palettes(section3_iterator, section3_size);
	if(state.palettes_ok) read_layers(section8_iterator, section8_size);

	state.dirty_menu = true;

	redraw();
}

void read_field_file(char *file)
{
	struct stat s;
	FILE *f;
	int size;
	void *buffer;
	struct field_header *header;

	if(stat(file, &s))
	{
		MessageBoxA(0, "Couldn't find file!", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	size = s.st_size;

	if(size > 0x2000000)
	{
		MessageBoxA(0, "Cowardly refusing to open file larger than 32MB.", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	f = fopen(file, "rb");

	if(!f)
	{
		MessageBoxA(0, "Couldn't open file!", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	buffer = malloc_read(f, size);

	header = (struct field_header *)buffer;

	if(header->unknown != 0 || header->sections != 9 || header->section0 != 0x2A)
	{
		void *buffer2;

		size *= 10;
		buffer2 = malloc(size);
		if(!(size = unpack_lzs((uint32_t)buffer, (uint32_t)buffer2, (uint32_t)buffer + size / 10)))
		{
			free(buffer2);
		}
		else
		{
			free(buffer);
			buffer = buffer2;

			header = (struct field_header *)buffer;
		}

		if(header->unknown != 0 || header->sections != 9 || header->section0 != 0x2A)
		{
			uint32_t ret = MessageBoxA(0, "This doesn't look like a field file. Open anyway?", "Warning", MB_ICONWARNING | MB_OKCANCEL);

			if(ret != IDCANCEL) parse_field_file(buffer, (uint32_t)buffer + size);
			else free(buffer);
		}
		else parse_field_file(buffer, (uint32_t)buffer + size);
	}
	else parse_field_file(buffer, (uint32_t)buffer + size);

	if(state.palettes_ok && state.layers_ok)
	{
		char title[4096];

		_splitpath(file, 0, 0, state.name, 0);

		sprintf(title, "Palmer - FF7 Field Background Editor (%s)", state.name);

		glutSetWindowTitle(title);
	}

	fclose(f);

	return;
}

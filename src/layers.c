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
#include <windows.h>
#include <gl/gl.h>
#include <assert.h>

#include "palmer.h"
#include "section.h"
#include "palettes.h"
#include "layers.h"

struct section8_header
{
	word unused1;
	word unknown1;
	unsigned char layer1_present;
	char unused2[7];
	char colorkey[20];
	word unused3;
	word unused4;
	char unused5[4];
	word width;
	word height;
	word layer1_tiles;
	word unknown3;
	word unused6;
	word unused7;
};

struct layer2_header
{
	word width;
	word height;
	word layer2_tiles;
	word unknown1;
	word unknown2;
	word layer2_start_page;
	word layer2_end_page;
	word unknown5;
	word unknown6;
	word unknown7;
	word unknown8;
	word unused1;
	word unused2;
};

struct layer3_header
{
	word width;
	word height;
	word layer3_tiles;
	word unknown1;
	word layer3_start_page;
	word layer3_end_page;
	word unknown4;
	word unknown5;
	word unused1;
	word unused2;
};

void read_layers(uint iterator, uint size)
{
	uint i;
	uint section_end = iterator + size;
	struct section8_header header;
	struct layer2_header layer2_header;
	struct layer3_header layer3_header;
	struct layer3_header layer4_header;
	unsigned char *tmp;

	section_read_bytes(&iterator, &header, sizeof(header));

	if(!header.layer1_present) MessageBoxA(0, "Layer 1 not present!", "Error", 0);

	state.layer1_width = header.width;
	state.layer1_height = header.height;
	state.layer1_tiles_num = header.layer1_tiles;
	memcpy(state.colorkey, header.colorkey, 20);

	state.layer1_tiles = (struct layer_tile *)iterator;
	iterator += state.layer1_tiles_num * sizeof(*state.layer1_tiles);

	state.layer2_present = section_read_byte(&iterator);

	if(state.layer2_present)
	{
		section_read_bytes(&iterator, &layer2_header, sizeof(layer2_header));
		state.layer2_width = layer2_header.width;
		state.layer2_height = layer2_header.height;
		state.layer2_tiles_num = layer2_header.layer2_tiles;
		state.layer2_tiles = (struct layer_tile *)iterator;
		iterator += state.layer2_tiles_num * sizeof(*state.layer2_tiles);
	}
	else state.layer2_tiles_num = 0;

	state.layer3_present = section_read_byte(&iterator);

	if(state.layer3_present)
	{
		state.parallax_back = true;
		section_read_bytes(&iterator, &layer3_header, sizeof(layer3_header));
		state.layer3_width = layer3_header.width;
		state.layer3_height = layer3_header.height;
		state.layer3_tiles_num = layer3_header.layer3_tiles;
		state.layer3_tiles = (struct layer_tile *)iterator;
		iterator += state.layer3_tiles_num * sizeof(*state.layer3_tiles);
	}
	else
	{
		state.layer3_tiles_num = 0;
		state.parallax_back = false;
	}

	state.layer4_present = section_read_byte(&iterator);

	if(state.layer4_present)
	{
		state.parallax_front = true;
		section_read_bytes(&iterator, &layer4_header, sizeof(layer4_header));
		state.layer4_width = layer4_header.width;
		state.layer4_height = layer4_header.height;
		state.layer4_tiles_num = layer4_header.layer3_tiles;
		state.layer4_tiles = (struct layer_tile *)iterator;
		iterator += state.layer4_tiles_num * sizeof(*state.layer4_tiles);
	}
	else
	{
		state.layer4_tiles_num = 0;
		state.parallax_front = false;
	}

	iterator += 7;

	tmp = malloc(256 * 256 * 4);

	for(i = 0; i < 42; i++)
	{
		state.layers[i].present = section_read_word(&iterator);

		if(!state.layers[i].present) continue;

		state.layers[i].unknown1 = section_read_word(&iterator);
		state.layers[i].type = section_read_word(&iterator);

		if(state.layers[i].type == 1)
		{
			uint j, k;

			state.layers[i].data = (void *)iterator;
			iterator += 256 * 256;

			glGenTextures(20, state.layers[i].textures);

			for(k = 0; k < state.palettes_num; k++)
			{
				for(j = 0; j < 256 * 256; j++)
				{
					unsigned char pixel = ((unsigned char *)state.layers[i].data)[j];
					word color = state.palettes[k][pixel];

					if(!color) color = state.palettes[k][0];

					tmp[j * 4 + 0] = ((color & 0x001F) * 255) / 31;
					tmp[j * 4 + 1] = (((color & 0x03E0) >> 5) * 255) / 31;
					tmp[j * 4 + 2] = (((color & 0x7C00) >> 10) * 255) / 31;
					tmp[j * 4 + 3] = 255; // - (((color & 0x8000) >> 15) * 255);

					if(state.colorkey[k] && pixel == 0) tmp[j * 4 + 3] = 0;
				}

				glBindTexture(GL_TEXTURE_2D, state.layers[i].textures[k]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
			}
		}

		if(state.layers[i].type == 2)
		{
			uint j;

			state.layers[i].data = (void *)iterator;
			iterator += 256 * 256 * 2;

			for(j = 0; j < 256 * 256; j++)
			{
				word color = ((word *)state.layers[i].data)[j];
				tmp[j * 4 + 0] = (((color & 0xF800) >> 11) * 255) / 31;
				tmp[j * 4 + 1] = (((color & 0x07E0) >> 5) * 255) / 63;
				tmp[j * 4 + 2] = (((color & 0x001F) >> 0) * 255) / 31;
				tmp[j * 4 + 3] = 255;

				if(color == 0) tmp[j * 4 + 3] = 0;
			}

			glGenTextures(1, state.layers[i].textures);
			glBindTexture(GL_TEXTURE_2D, state.layers[i].textures[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
		}
	}

	free(tmp);

	if(strncmp("END", (char *)iterator, 3)) MessageBoxA(0, "Didn't hit end tag in background section.", "Warning", 0);

	iterator += 3;

	if(iterator == section_end) state.layers_ok = true;
	else
	{
		MessageBoxA(0, "Read past end of background section.", "Error", 0);
		state.layers_ok = false;
	}
}

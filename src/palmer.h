/****************************************************************************/
//    Copyright (C) 2014 Aali132                                            //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#pragma once

#include <cstdint>
#include <sstream>
#include <windows.h>
#include <gl/gl.h>
#include <stdlib.h>

#pragma pack(1)

#define true 1
#define false 0

#define NORMAL_LAYERS 256
#define ANIMATION_LAYERS (64 * 256 * 4)
#define ADDITIVE_LAYERS ANIMATION_LAYERS

#define EXPORT_LAYERS NORMAL_LAYERS + ANIMATION_LAYERS + ADDITIVE_LAYERS

#define PALETTES 20
#define FF7_LAYERS 42

void redraw();
void rebuild_util_menu(void);

struct field_header
{
	uint16_t unknown;
	uint32_t sections;
	uint32_t section0;
	uint32_t section1;
	uint32_t section2;
	uint32_t section3;
	uint32_t section4;
	uint32_t section5;
	uint32_t section6;
	uint32_t section7;
	uint32_t section8;
};

struct layer
{
	uint16_t present;
	uint16_t unknown1;
	uint16_t type;
	void *data;
	GLuint textures[PALETTES];
	uint32_t imported_width;
	uint32_t imported_height;
	uint32_t *imported_data;
};

struct layer_tile
{
	short x;
	short y;
	uint16_t unknown1;
	uint16_t unknown2;
	uint16_t source_x;
	uint16_t source_y;
	uint16_t fx_source_x;
	uint16_t fx_source_y;
	uint16_t tile_size_x;
	uint16_t tile_size_y;
	uint16_t palette_index;
	uint16_t flags;
	uint8_t anim_group;
	uint8_t anim_bitmask;
	uint8_t use_fx_page;
	uint8_t unused1;
	uint16_t blend_mode;
	uint16_t source_page;
	uint16_t fx_page;
	uint16_t unknown4;
	uint32_t fixed_z;
	uint32_t fixed_u;
	uint32_t fixed_v;
	uint16_t unused2;
	uint16_t unused3;
};

struct field_state
{
	uint32_t layer1_width;
	uint32_t layer1_height;
	uint32_t layer1_tiles_num;
	struct layer_tile *layer1_tiles;
	uint32_t layer2_width;
	uint32_t layer2_height;
	bool layer2_present;
	uint32_t layer2_tiles_num;
	struct layer_tile *layer2_tiles;
	uint32_t layer3_width;
	uint32_t layer3_height;
	bool layer3_present;
	uint32_t layer3_tiles_num;
	struct layer_tile *layer3_tiles;
	uint32_t layer4_width;
	uint32_t layer4_height;
	bool layer4_present;
	uint32_t layer4_tiles_num;
	struct layer_tile *layer4_tiles;
	char colorkey[PALETTES];
	struct layer layers[FF7_LAYERS];
	bool parallax_back;
	bool parallax_front;
	volatile bool layers_ok ;
	uint16_t *palettes[PALETTES];
	uint32_t palettes_num;
	uint32_t palette_colors;
	volatile bool palettes_ok;
	int current_layer;
	uint32_t used_anim_state[64];
	uint32_t anim_state[64];
	int x_offset;
	int y_offset;
	int scale;
	char name[_MAX_FNAME];
	bool dirty_menu;
	bool first_pass_done;
	struct layer_tile **export_layers[EXPORT_LAYERS];
	int export_extents[EXPORT_LAYERS][4];
	int export_tile_size;
};

extern struct field_state state;

extern volatile bool dialog;

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

#ifndef _PALMER_H_
#define _PALMER_H_

#include <windows.h>
#include <gl/gl.h>
#include <stdlib.h>

#pragma pack(1)

typedef unsigned short word;
typedef unsigned int uint;
typedef unsigned int bool;

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
	word unknown;
	uint sections;
	uint section0;
	uint section1;
	uint section2;
	uint section3;
	uint section4;
	uint section5;
	uint section6;
	uint section7;
	uint section8;
};

struct layer
{
	word present;
	word unknown1;
	word type;
	void *data;
	GLuint textures[PALETTES];
	uint imported_width;
	uint imported_height;
	uint *imported_data;
};

struct layer_tile
{
	short x;
	short y;
	word unknown1;
	word unknown2;
	word source_x;
	word source_y;
	word fx_source_x;
	word fx_source_y;
	word tile_size_x;
	word tile_size_y;
	word palette_index;
	word flags;
	unsigned char anim_group;
	unsigned char anim_bitmask;
	unsigned char use_fx_page;
	unsigned char unused1;
	word blend_mode;
	word source_page;
	word fx_page;
	word unknown4;
	uint fixed_z;
	uint fixed_u;
	uint fixed_v;
	word unused2;
	word unused3;
};

struct field_state
{
	uint layer1_width;
	uint layer1_height;
	uint layer1_tiles_num;
	struct layer_tile *layer1_tiles;
	uint layer2_width;
	uint layer2_height;
	bool layer2_present;
	uint layer2_tiles_num;
	struct layer_tile *layer2_tiles;
	uint layer3_width;
	uint layer3_height;
	bool layer3_present;
	uint layer3_tiles_num;
	struct layer_tile *layer3_tiles;
	uint layer4_width;
	uint layer4_height;
	bool layer4_present;
	uint layer4_tiles_num;
	struct layer_tile *layer4_tiles;
	char colorkey[PALETTES];
	struct layer layers[FF7_LAYERS];
	bool parallax_back;
	bool parallax_front;
	volatile bool layers_ok ;
	word *palettes[PALETTES];
	uint palettes_num;
	uint palette_colors;
	volatile bool palettes_ok;
	int current_layer;
	uint used_anim_state[64];
	uint anim_state[64];
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

#endif

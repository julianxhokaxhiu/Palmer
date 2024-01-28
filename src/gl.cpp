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
#include <gl/glew.h>

#include "palmer.h"
#include "layers.h"
#include "palettes.h"
#include "layers.h"

extern bool no_blend_equation;

void gl_init()
{
	glewInit();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
}

void export_tile(struct layer_tile *tile, int size, uint32_t layer)
{
	int tile_x, tile_y;
	int x, y;
	uint32_t i = 0;

	tile_x = tile->x + abs(tile->x % 16);
	tile_y = tile->y + abs(tile->y % 16);

	x = 64 + (tile_x / 16);
	y = 64 + (tile_y / 16);

	if(x >= 128 || y >= 128 || x < 0 || y < 0) return;

	if(layer == 2) i += NORMAL_LAYERS / 2;

	if(tile->anim_group) i += NORMAL_LAYERS + (tile->anim_group + tile->anim_bitmask * 64) * (ANIMATION_LAYERS / (64 * 256));
	if(tile->fx_page) i += ANIMATION_LAYERS;

	for(; i < EXPORT_LAYERS; i++)
	{
		if(!state.export_layers[i]) state.export_layers[i] = (layer_tile**)calloc(128 * 128, sizeof(*state.export_layers));

		if(!state.export_layers[i][x * 128 + y])
		{
			state.export_layers[i][x * 128 + y] = tile;
			if(tile_x < state.export_extents[i][0]) state.export_extents[i][0] = tile_x;
			if(tile_x + size > state.export_extents[i][1]) state.export_extents[i][1] = tile_x + size;
			if(tile_y < state.export_extents[i][2]) state.export_extents[i][2] = tile_y;
			if(tile_y + size > state.export_extents[i][3]) state.export_extents[i][3] = tile_y + size;
			return;
		}
	}

	MessageBoxA(0, "Overflow", "Error", MB_ICONERROR | MB_OK);
}

void render_layer(struct layer_tile *tiles, uint32_t num, uint32_t tile_size, uint32_t layer)
{
	uint32_t pass, i;

	state.export_tile_size = tile_size;

	for(pass = 0; pass < 2; pass++)
	{
		for(i = 0; i < num; i++)
		{
			struct layer_tile *tile = &tiles[i];
			float x1 = (float)tile->x;
			float x2 = (float)tile->x + tile_size;
			float y1 = (float)tile->y;
			float y2 = (float)tile->y + tile_size;
			float u1 = tile->source_x / 256.0f;
			float u2 = (tile->source_x + tile_size) / 256.0f;
			float v1 = tile->source_y / 256.0f;
			float v2 = (tile->source_y + tile_size) / 256.0f;
			uint32_t page = tile->source_page;
			float z;

			if(!no_blend_equation) glBlendEquation(GL_FUNC_ADD);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glBlendFunc(GL_ONE, GL_ZERO);
			glDepthMask(GL_TRUE);

			if(tile->use_fx_page)
			{
				if(pass == 0) continue;

				u1 = tile->fx_source_x / 256.0f;
				u2 = (tile->fx_source_x + tile_size) / 256.0f;
				v1 = tile->fx_source_y / 256.0f;
				v2 = (tile->fx_source_y + tile_size) / 256.0f;

				page = tile->fx_page;

				if(page >= 42) continue;

				switch(tile->blend_mode)
				{
					case 0:
						glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glDepthMask(GL_FALSE);
						if(layer < 3 && state.layers[page].type == 1 && page < 0x18) MessageBoxA(0, "Wrong page for blend mode 0\n", "Glitch", MB_ICONWARNING | MB_OK);
						break;
					case 1:
						glBlendFunc(GL_ONE, GL_ONE);
						glDepthMask(GL_FALSE);
						if(layer < 3 && state.layers[page].type == 1 && page < 0xF || page >= 0x18) MessageBoxA(0, "Wrong page for blend mode 1\n", "Glitch", MB_ICONWARNING | MB_OK);
						break;
					case 2:
						if(!no_blend_equation) glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
						glBlendFunc(GL_ONE, GL_ONE);
						glDepthMask(GL_FALSE);
						break;
					case 3:
						glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE);
						glDepthMask(GL_FALSE);
						break;
					default:
						MessageBoxA(0, "Unknown blend mode\n", "Glitch", MB_ICONWARNING | MB_OK);
				}
			}
			else if(pass == 1) continue;

			if(page >= 42) continue;

			if(tile->anim_group >= 64) tile->anim_group = 0;

			export_tile(tile, tile_size, layer);

			if(tile->anim_group)
			{
				if(tile->anim_bitmask) state.used_anim_state[tile->anim_group] |= tile->anim_bitmask;
				if(!(state.anim_state[tile->anim_group] & tile->anim_bitmask)) continue;
			}

			if(layer == 1) z = 0.999999f;
			else z = (float)(tile->fixed_z / 10000000.0);

			if(state.layers[page].type == 1)
			{
				if(tile->palette_index >= state.palettes_num) continue;
				glBindTexture(GL_TEXTURE_2D, state.layers[page].textures[tile->palette_index]);
			}
			else glBindTexture(GL_TEXTURE_2D, state.layers[page].textures[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glBegin(GL_QUADS);
			glTexCoord2f(u1, v1); glVertex3f(x1, y1, z);
			glTexCoord2f(u2, v1); glVertex3f(x2, y1, z);
			glTexCoord2f(u2, v2); glVertex3f(x2, y2, z);
			glTexCoord2f(u1, v2); glVertex3f(x1, y2, z);
			glEnd();
		}
	}
}

void gl_render_scene()
{
	uint32_t i;

	if(!state.first_pass_done)
	{
		render_layer(state.layer1_tiles, state.layer1_tiles_num, 16, 1);
		render_layer(state.layer2_tiles, state.layer2_tiles_num, 16, 2);
		render_layer(state.layer3_tiles, state.layer3_tiles_num, 32, 3);
		render_layer(state.layer4_tiles, state.layer4_tiles_num, 32, 4);

		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		state.first_pass_done = true;
	}

	for(i = 0; i < EXPORT_LAYERS; i++)
	{
		if(state.export_layers[i]) free(state.export_layers[i]);
	}

	memset(state.export_layers, 0, sizeof(state.export_layers));
	memset(state.export_extents, 0, sizeof(state.export_extents));

	if(state.current_layer == 0)
	{
		render_layer(state.layer1_tiles, state.layer1_tiles_num, 16, 1);
		render_layer(state.layer2_tiles, state.layer2_tiles_num, 16, 2);
	}

	else if(state.current_layer == 1) render_layer(state.layer3_tiles, state.layer3_tiles_num, 32, 3);
	else if(state.current_layer == 2) render_layer(state.layer4_tiles, state.layer4_tiles_num, 32, 4);
}

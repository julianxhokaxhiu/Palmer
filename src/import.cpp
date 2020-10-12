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

#include <stdio.h>
#include <libpng16/png.h>
#include <direct.h>

#include "palmer.h"
#include "export.h"

uint32_t *read_png(char *filename, uint32_t *_width, uint32_t *_height)
{
	png_bytepp rowptrs;
	FILE *f;
	png_infop info_ptr;
	png_structp png_ptr;
	uint32_t width;
	uint32_t height;
	uint32_t *data;
	uint32_t y;
	uint32_t color_type;

	if(fopen_s(&f, filename, "rb")) return 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, 0, 0);

	if(!png_ptr)
	{
		fclose(f);
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(f);
		return 0;
	}

	png_init_io(png_ptr, f);

	png_read_png(png_ptr, info_ptr, 0, NULL);

	if(png_get_bit_depth(png_ptr, info_ptr) != 8) return 0;

	color_type = png_get_color_type(png_ptr, info_ptr);

	if(color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA) return 0;

	rowptrs = png_get_rows(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	*_width = width;
	height = png_get_image_height(png_ptr, info_ptr);
	*_height = height;

	data = (uint32_t*)malloc(width * height * 4);

	if(color_type == PNG_COLOR_TYPE_RGB)
	{
		uint32_t x;

		for(y = 0; y < height; y++)
		{
			uint32_t o = (uint32_t)rowptrs[y];

			for(x = 0; x < width; x++)
			{
				uint32_t r = (*(uint8_t *)o++);
				uint32_t g = (*(uint8_t *)o++);
				uint32_t b = (*(uint8_t *)o++);

				data[y * width + x] = r | g << 8 | b << 16 | 0xFF000000;
			}
		}
	}

	else for(y = 0; y < height; y++) memcpy(&data[y * width], rowptrs[y], width * 4);

	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	fclose(f);

	return data;
}

bool import_png(char *name, bool fullpath)
{
	int i, j, k, l, m;
	int extents[4];
	uint32_t width, height;
	int org_width, org_height;
	int tilefact_x = 0;
	int tilefact_y = 0;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	if(fullpath) _splitpath(name, drive, dir, 0, 0);

	extents[0] = 0;
	extents[1] = 0;
	extents[2] = 0;
	extents[3] = 0;

	for(i = 0; i < EXPORT_LAYERS; i++)
	{
		if(state.export_extents[i][0] < extents[0]) extents[0] = state.export_extents[i][0];
		if(state.export_extents[i][1] > extents[1]) extents[1] = state.export_extents[i][1];
		if(state.export_extents[i][2] < extents[2]) extents[2] = state.export_extents[i][2];
		if(state.export_extents[i][3] > extents[3]) extents[3] = state.export_extents[i][3];
	}

	org_width = extents[1] - extents[0];
	org_height = extents[3] - extents[2];

	for(i = 0; i < EXPORT_LAYERS; i++)
	{
		uint32_t *pixels;
		char filename[4096];

		if(!state.export_layers[i]) continue;

		if(fullpath) sprintf(filename, "%s/%s/%s_%i_%08i.png", drive, dir, state.name, state.current_layer, i);
		else sprintf(filename, "%s_batch/%s_%i_%08i.png", state.name, state.name, state.current_layer, i);

		if(!(pixels = (uint32_t *)read_png(filename, &width, &height)))
		{
			std::ostringstream tmp;
			tmp << "Missing image: " << filename;

			MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);

			return false;
		}

		if(tilefact_x == 0 && tilefact_y == 0)
		{
			tilefact_x = width / org_width;
			tilefact_y = height / org_height;

			for(l = 0; l < 42; l++)
			{
				if(state.layers[l].present)
				{
					if(state.layers[l].imported_data) continue;

					state.layers[l].imported_width = 256 * tilefact_x;
					state.layers[l].imported_height = 256 * tilefact_y;
				}
			}
		}

		if((tilefact_x * org_width != width) || (tilefact_y * org_height != height))
		{
			std::ostringstream tmp;

			tmp << "Imported image \"" << filename << "\" is " << width << "x" << height << "pixels, expected " << tilefact_x * org_width << "x" << tilefact_y * org_height << ".";

			MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);

			return false;
		}

		for(j = 0; j < width / (16 * tilefact_x); j++)
		{
			for(k = 0; k < height / (16 * tilefact_y); k++)
			{
				uint32_t x = 64 + (j * 16 + extents[0]) / 16;
				uint32_t y = 64 + (k * 16 + extents[2]) / 16;
				struct layer_tile *tile = state.export_layers[i][x * 128 + y];
				uint32_t page, src_x, src_y;

				if(!tile) continue;

				if(tile->fx_page)
				{
					page = tile->fx_page;
					src_x = tile->fx_source_x;
					src_y = tile->fx_source_y;
				}
				else
				{
					page = tile->source_page;
					src_x = tile->source_x;
					src_y = tile->source_y;
				}

				if(state.layers[page].imported_width != 256 * tilefact_x || state.layers[page].imported_height != 256 * tilefact_y)
				{
					MessageBoxA(0, "Imported data conflicts with previous import.\nRe-open field file to clear all imports.", "Error", MB_ICONERROR | MB_OK);
					continue;
				}

				for(l = 0; l < state.export_tile_size * tilefact_x; l++)
				{
					for(m = 0; m < state.export_tile_size * tilefact_y; m++)
					{
						uint32_t color = pixels[j * (16 * tilefact_x) + l + (k * (16 * tilefact_y) + m) * width];

						if(!state.layers[page].imported_data) state.layers[page].imported_data = (uint32_t*)calloc(256 * tilefact_x * 256 * tilefact_y, 4);
						state.layers[page].imported_data[(src_x * tilefact_x) + l + ((src_y * tilefact_y) + m) * (256 * tilefact_y)] = color;
					}
				}
			}
		}

		free(pixels);
	}

	for(l = 0; l < FF7_LAYERS; l++)
	{
		for(m = 0; m < PALETTES; m++)
		{
			glBindTexture(GL_TEXTURE_2D, state.layers[l].textures[m]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, state.layers[l].imported_width, state.layers[l].imported_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, state.layers[l].imported_data);
		}
	}

	return true;
}

void write_imported_layers(char *name, bool fullpath)
{
	uint32_t i;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	bool found_imported_layer = false;

	if(fullpath) _splitpath(name, drive, dir, 0, 0);

	for(i = 0; i < FF7_LAYERS; i++)
	{
		uint32_t width = state.layers[i].imported_width;
		uint32_t height = state.layers[i].imported_height;

		if(!state.layers[i].present) continue;

		if(state.layers[i].imported_data)
		{
			char filename[1024];

			found_imported_layer = true;

			if(fullpath) sprintf(filename, "%s/%s/%s_%02i_00.png", drive, dir, state.name, i);
			else
			{
				mkdir("output");
				sprintf(filename, "output/%s", state.name);
				mkdir(filename);
				sprintf(filename, "output/%s/%s_%02i_00.png", state.name, state.name, i);
			}

			write_png(filename, width, height, (char *)state.layers[i].imported_data);
		}
	}

	std::ostringstream tmp;

	if(!found_imported_layer) tmp << "You haven't imported anything yet";
	else tmp << "Place PNG files in mods/<modpath>/field/" << state.name << "/";

	if(fullpath) MessageBoxA(0, tmp.str().c_str(), "Warning", MB_ICONWARNING | MB_OK);
}

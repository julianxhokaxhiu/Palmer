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

#include <stdio.h>
#include <libpng16/png.h>
#include <direct.h>

#include "palmer.h"

void make_path(char *name)
{
	char *next = name;

	while((next = strchr(next, '/')))
	{
		char tmp[128];

		while(next[0] == '/') next++;

		strncpy(tmp, name, next - name);
		tmp[next - name] = 0;

		mkdir(tmp);
	}
}

bool write_png(char *filename, uint32_t width, uint32_t height, char *data)
{
	uint32_t y;
	png_bytepp rowptrs;
	FILE *f;
	png_infop info_ptr;
	png_structp png_ptr;

	if(fopen_s(&f, filename, "wb"))
	{
		printf("couldn't open file %s for writing: %s", filename, _strerror(NULL));
		return false;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, 0, 0);

	if(!png_ptr)
	{
		fclose(f);
		return false;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fclose(f);
		return false;
	}

	png_init_io(png_ptr, f);

	png_set_compression_level(png_ptr, 1);

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	rowptrs = (png_bytepp)malloc(height * 4);

	for(y = 0; y < height; y++) rowptrs[y] = (png_bytep)&data[y * width * 4];

	png_set_rows(png_ptr, info_ptr, rowptrs);

	png_write_png(png_ptr, info_ptr, 0, NULL);

	png_destroy_write_struct(&png_ptr, &info_ptr);

	free(rowptrs);

	fclose(f);

	return true;
}

bool export_png(char *name, bool fullpath)
{
	int i, j, k, l, m;
	int extents[4];
	int width, height;
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

	width = extents[1] - extents[0];
	height = extents[3] - extents[2];

	for(i = 0; i < EXPORT_LAYERS; i++)
	{
		uint32_t *pixels;
		char filename[4096];

		if(!state.export_layers[i]) continue;

		pixels = (uint32_t*)calloc(width * height, 4);

		for(j = 0; j < width / 16; j++)
		{
			for(k = 0; k < height / 16; k++)
			{
				uint32_t x = 64 + (j * 16 + extents[0]) / 16;
				uint32_t y = 64 + (k * 16 + extents[2]) / 16;
				struct layer_tile *tile = state.export_layers[i][x * 128 + y];
				uint32_t page, palette_index, src_x, src_y;

				if(!tile) continue;

				palette_index = tile->palette_index;

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

				if(state.layers[page].type == 1 && tile->palette_index >= state.palettes_num) continue;

				for(l = 0; l < state.export_tile_size; l++)
				{
					for(m = 0; m < state.export_tile_size; m++)
					{
						uint32_t color;

						if(state.layers[page].type == 1)
						{
							uint8_t pixel = ((uint8_t *)state.layers[page].data)[src_x + l + (src_y + m) * 256];
							uint16_t pal_color = state.palettes[palette_index][pixel];

							if(state.colorkey[palette_index] && pixel == 0) color = 0;
							else
							{
								if(!pal_color) pal_color = state.palettes[palette_index][0];

								color = ((pal_color & 0x001F) * 255) / 31;
								color |= ((((pal_color & 0x03E0) >> 5) * 255) / 31) << 8;
								color |= ((((pal_color & 0x7C00) >> 10) * 255) / 31) << 16;
								color |= 255 << 24; // - (((pal_color & 0x8000) >> 15) * 255);
							}
						}

						if(state.layers[page].type == 2)
						{
							uint16_t pal_color = ((uint16_t *)state.layers[page].data)[src_x + l + (src_y + m) * 256];

							if(pal_color == 0) color = 0;
							else
							{
								color = (((pal_color & 0xF800) >> 11) * 255) / 31;
								color |= ((((pal_color & 0x07E0) >> 5) * 255) / 63) << 8;
								color |= ((((pal_color & 0x001F) >> 0) * 255) / 31) << 16;
								color |= 255 << 24;
							}
						}

						pixels[j * 16 + l + (k * 16 + m) * width] = color;
					}
				}
			}
		}

		if(fullpath) sprintf(filename, "%s/%s/%s_%i_%08i.png", drive, dir, state.name, state.current_layer, i);
		else
		{
			sprintf(filename, "%s_batch", state.name);
			mkdir(filename);
			sprintf(filename, "%s_batch/%s_%i_%08i.png", state.name, state.name, state.current_layer, i);
		}

		if(!write_png(filename, width, height, (char *)pixels))
		{
			std::ostringstream tmp;
			tmp << "Couldn't write PNG: " << filename;

			MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);
			free(pixels);
			return false;
		}

		free(pixels);
	}

	return true;
}

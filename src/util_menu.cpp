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

#include <gl/freeglut.h>
#include <stdio.h>

#include "palmer.h"

int util_menu;

void util_menu_cb(int entry)
{
	if(dialog) return;
}

void layer_menu_cb(int entry)
{
	if(dialog) return;

	if(entry == -1) return;

	state.current_layer = entry;
	state.dirty_menu = true;
}

void anim_menu_cb(int entry)
{
	char tmp[1024];

	if(dialog) return;

	if(entry == -1) return;

	state.anim_state[entry / 8] ^= (1 << (entry % 8));

	sprintf(tmp, "State %i - %s", entry % 8 + 1, (state.anim_state[entry / 8] & (1 << (entry % 8))) ? "On" : "Off");

	glutChangeToMenuEntry(entry % 8 + 1, tmp, entry);
}

int layer_menu;
int anim_menu[7];
int anim_menus[64];

void rebuild_util_menu(void)
{
	uint32_t i;
	uint32_t j;
	uint32_t x;
	char tmp[1024];

	if(util_menu == 0)
	{
		util_menu = glutCreateMenu(layer_menu_cb);
		glutAttachMenu(GLUT_RIGHT_BUTTON);

		layer_menu = glutCreateMenu(layer_menu_cb);
		glutAddMenuEntry("Normal Layer", 0);
		glutAddMenuEntry("dummy", -1);
		glutAddMenuEntry("dummy", -1);

		glutSetMenu(util_menu);
		glutAddSubMenu("Layers", layer_menu);

		for(x = 0; x < 7; x++)
		{
			anim_menu[x] = glutCreateMenu(anim_menu_cb);

			for(i = x * 10 + 1; i < (x + 1) * 10 + 1 && i < 64; i++)
			{
				anim_menus[i] = glutCreateMenu(anim_menu_cb);

				for(j = 0; j < 8; j++) glutAddMenuEntry("dummy", -1);

				glutSetMenu(anim_menu[x]);
				glutAddSubMenu("dummy", anim_menus[i]);
			}

			glutSetMenu(util_menu);
			sprintf(tmp, "Animations %i-%i", x * 10 + 1, (x + 1) * 10);
			glutAddSubMenu(tmp, anim_menu[x]);
		}
	}

	glutSetMenu(layer_menu);

	if(state.parallax_back && state.layer3_tiles_num) glutChangeToMenuEntry(2, "Parallax background layer", 1);
	else glutChangeToMenuEntry(2, "Unused", -1);

	if(state.parallax_front && state.layer4_tiles_num) glutChangeToMenuEntry(3, "Parallax foreground layer", 2);
	else glutChangeToMenuEntry(3, "Unused", -1);

	for(x = 0; x < 7; x++)
	{
		glutSetMenu(anim_menu[x]);

		for(i = x * 10 + 1; i < (x + 1) * 10 + 1 && i < 64; i++)
		{
			if(state.used_anim_state[i])
			{
				sprintf(tmp, "Group %i", i);
				glutChangeToSubMenu(i - 10 * x, tmp, anim_menus[i]);

				glutSetMenu(anim_menus[i]);

				for(j = 0; j < 8; j++)
				{
					if(state.used_anim_state[i] & (1 << j))
					{
						sprintf(tmp, "State %i - %s", j + 1, state.anim_state[i] & (1 << j) ? "On" : "Off");
						glutChangeToMenuEntry(j + 1, tmp, i * 8 + j);
					}
					else glutChangeToMenuEntry(j + 1, "Unused", -1);
				}

				glutSetMenu(anim_menu[x]);
			}

			else glutChangeToMenuEntry(i - 10 * x, "Unused", -1);
		}
	}
}

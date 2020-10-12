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
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <stdio.h>
#include <sys/stat.h>

#include "palmer.h"
#include "palmer_gl.h"
#include "file.h"
#include "export.h"
#include "import.h"

volatile bool dialog = false;

bool no_blend_equation = false;

int mouse_x, mouse_y;
bool mouse_scroll;

struct field_state state;

HKEY ff7_regkey;
char filename[4096];
uint32_t filename_length = sizeof(filename);
char basedir[4096];
DWORD basedir_length = sizeof(basedir);

#pragma pack(1)

void redraw()
{
	glutPostRedisplay();
}

void main_display_cb(void)
{
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glScalef((float)state.scale, (float)state.scale, 1.0f);
	glTranslatef((float)state.x_offset, (float)state.y_offset, 0.0f);

	if(state.palettes_ok && state.layers_ok) gl_render_scene();

	if(state.dirty_menu) rebuild_util_menu();

	state.dirty_menu = false;

	glutSwapBuffers();
}

void main_reshape_cb(int width, int height)
{
	int x = width / 2, y = height / 2;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-x, width - x, height - y, -y, 1.0, -1.0);
	glMatrixMode(GL_MODELVIEW);
}

void mouse_button_cb(int button, int state, int x, int y)
{
	if(button != GLUT_MIDDLE_BUTTON) return;

	mouse_x = x;
	mouse_y = y;

	mouse_scroll = (state == GLUT_DOWN);
}

void mouse_move_cb(int x, int y)
{
	if(mouse_scroll)
	{
		state.x_offset += mouse_x - x;
		state.y_offset += mouse_y - y;

		mouse_x = x;
		mouse_y = y;

		redraw();
	}
}

void mouse_zoom_cb(int wheel, int direction, int x, int y)
{
	if(direction == 1)
	{
		if(state.scale < 128) state.scale *= 2;
	}
	else if(state.scale > 1) state.scale /= 2;

	redraw();
}

void main_menu_cb(int entry)
{
	if(dialog) return;

	switch(entry)
	{
		OPENFILENAMEA ofn;
		int i;

		case 0:
			memset(&ofn, 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = filename;
			ofn.nMaxFile = sizeof(filename);
			ofn.lpstrInitialDir = basedir;
			dialog = true;
			if(GetOpenFileName(&ofn)) read_field_file(ofn.lpstrFile);
			dialog = false;
			break;
		case 1:
			memset(&ofn, 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = filename;
			ofn.nMaxFile = sizeof(filename);
			ofn.lpstrInitialDir = basedir;
			dialog = true;
			if(GetSaveFileName(&ofn)) write_imported_layers(ofn.lpstrFile, true);
			dialog = false;
			break;
		case 2:
			memset(&ofn, 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = filename;
			ofn.nMaxFile = sizeof(filename);
			ofn.lpstrInitialDir = basedir;
			dialog = true;
			if(GetOpenFileName(&ofn))
			{
				uint32_t saved_layer = state.current_layer;

				state.current_layer = 0;
				gl_render_scene();
				if(!import_png(ofn.lpstrFile, true)) MessageBoxA(0, "Import failed. (Normal layer)", "Error", MB_ICONERROR | MB_OK);

				state.current_layer = 1;
				gl_render_scene();
				if(!import_png(ofn.lpstrFile, true)) MessageBoxA(0, "Import failed. (Parallax background layer)", "Error", MB_ICONERROR | MB_OK);

				state.current_layer = 2;
				gl_render_scene();
				if(!import_png(ofn.lpstrFile, true)) MessageBoxA(0, "Import failed. (Parallax foreground layer)", "Error", MB_ICONERROR | MB_OK);

				state.current_layer = saved_layer;
			}
			dialog = false;
			break;
		case 3:
			memset(&ofn, 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = filename;
			ofn.nMaxFile = sizeof(filename);
			ofn.lpstrInitialDir = basedir;
			dialog = true;
			if(GetSaveFileName(&ofn))
			{
				uint32_t saved_layer = state.current_layer;

				state.current_layer = 0;
				gl_render_scene();
				export_png(ofn.lpstrFile, true);

				state.current_layer = 1;
				gl_render_scene();
				export_png(ofn.lpstrFile, true);

				state.current_layer = 2;
				gl_render_scene();
				export_png(ofn.lpstrFile, true);

				state.current_layer = saved_layer;
			}
			dialog = false;
			break;
		case 6:
			dialog = true;
			for(i = 1; i < __argc; i++)
			{
				read_field_file(__argv[i]);

				if(state.palettes_ok && state.layers_ok)
				{
					uint32_t saved_layer = state.current_layer;

					state.current_layer = 0;
					gl_render_scene();
					if(!export_png(__argv[i], false))
					{
						std::ostringstream tmp;

						tmp << "Export failed for " << __argv[i] << ". (Normal layer)";

						MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);
						break;
					}

					state.current_layer = 1;
					gl_render_scene();
					if(!export_png(__argv[i], false))
					{
						std::ostringstream tmp;

						tmp << "Export failed for " << __argv[i] << ". (Parallax background layer)";

						MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);
						break;
					}

					state.current_layer = 2;
					gl_render_scene();
					if(!export_png(__argv[i], false))
					{
						std::ostringstream tmp;

						tmp << "Export failed for " << __argv[i] << ". (Parallax foreground layer)";

						MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);
						break;
					}

					state.current_layer = saved_layer;


				}
			}

			if(__argc == 1) MessageBoxA(0, "No filenames specified on command line", "Error", MB_ICONERROR | MB_OK);

			dialog = false;
			break;
		case 7:
			dialog = true;
			for(i = 1; i < __argc; i++)
			{
				read_field_file(__argv[i]);

				if(state.palettes_ok && state.layers_ok)
				{
					uint32_t saved_layer = state.current_layer;

					state.current_layer = 0;
					gl_render_scene();
					if(!import_png(__argv[i], false))
					{
						std::ostringstream tmp;

						tmp << "Export failed for " << __argv[i] << ". (Normal layer)";

						MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);
						break;
					}

					write_imported_layers(__argv[i], false);

					state.current_layer = 1;
					gl_render_scene();
					if(!import_png(__argv[i], false))
					{
						std::ostringstream tmp;

						tmp << "Export failed for " << __argv[i] << ". (Parallax background layer)";

						MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);
						break;
					}

					write_imported_layers(__argv[i], false);

					state.current_layer = 2;
					gl_render_scene();
					if(!import_png(__argv[i], false))
					{
						std::ostringstream tmp;

						tmp << "Export failed for " << __argv[i] << ". (Parallax foreground layer)";

						MessageBoxA(0, tmp.str().c_str(), "Error", MB_ICONERROR | MB_OK);
						break;
					}

					write_imported_layers(__argv[i], false);

					state.current_layer = saved_layer;
				}
			}

			if(__argc == 1) MessageBoxA(0, "No filenames specified on command line", "Error", MB_ICONERROR | MB_OK);

			dialog = false;
			break;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Square Soft, Inc.\\Final Fantasy VII", 0, KEY_QUERY_VALUE, &ff7_regkey);
	RegQueryValueEx(ff7_regkey, "AppPath", 0, 0, (LPBYTE)basedir, &basedir_length);
	basedir[sizeof(basedir) - 1] = 0;

	glutInit(&__argc, __argv);

	glutInitWindowSize(640, 480);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("Palmer - FF7 Field Background Editor");

	gl_init();

	glutDisplayFunc(main_display_cb);
	glutReshapeFunc(main_reshape_cb);
	glutMouseFunc(mouse_button_cb);
	glutMotionFunc(mouse_move_cb);
	glutMouseWheelFunc(mouse_zoom_cb);

	glutCreateMenu(main_menu_cb);
	glutAddMenuEntry("Open Field File", 0);
	glutAddMenuEntry("Export PNG Files", 3);
	glutAddMenuEntry("Import PNG Files", 2);
	glutAddMenuEntry("Write Modpath Textures", 1);
	glutAddMenuEntry("Batch Export", 6);
	glutAddMenuEntry("Batch Import", 7);

	glutAttachMenu(GLUT_LEFT_BUTTON);

	if(!glewIsSupported("GL_VERSION_1_2"))
	{
		no_blend_equation = true;
		MessageBoxA(0, "No support for subtractive blending, some effects will be missing.", "Warning", MB_ICONWARNING | MB_OK);
	}

	glutMainLoop();
}

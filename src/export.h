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

#include "palmer.h"

bool write_png(char *filename, uint32_t width, uint32_t height, char *data);
bool export_png(char *name, bool fullpath);

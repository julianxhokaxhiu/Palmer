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

#pragma once

#include "palmer.h"

void section_read_bytes(uint32_t *iterator, void *dest, uint32_t bytes);

template <class T>
T section_read(uint32_t *iterator)
{
  T ret;

  section_read_bytes(iterator, &ret, sizeof(ret));

  return ret;
}

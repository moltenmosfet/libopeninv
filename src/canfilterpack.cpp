/*
 * This file is part of the libopeninv project.
 *
 * Copyright (C) 2016 Nail Güzel
 * Johannes Huebner <dev@johanneshuebner.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <libopencm3/stm32/can.h>
#include "stm32_can.h"

/* Stm32Can::PackFilters, SetFilterBank, SetFilterBankMask and SetFilterBank29:
 * split out of stm32_can.cpp because they contain no hardware register
 * access (unlike the rest of that file, which pulls in ARM-specific inline
 * asm via cortex.h through Send()), so they can be compiled and exercised
 * directly by the host test suite. */

#define IDS_PER_BANK          4
#define EXT_IDS_PER_BANK      2

void Stm32Can::SetFilterBank(int& idIndex, int& filterId, uint16_t* idList)
{
   if (idIndex > 0 && idIndex < IDS_PER_BANK)
   {
      //half-filled bank: repeat the last real ID instead of leaving 0
      //(id 0 in list mode also matches ID 0)
      for (int i = idIndex; i < IDS_PER_BANK; i++)
         idList[i] = idList[idIndex - 1];
   }

   can_filter_id_list_16bit_init(
         filterId,
         idList[0] << 5, //left align
         idList[1] << 5,
         idList[2] << 5,
         idList[3] << 5,
         filterId & 1,
         true);
   idIndex = 0;
   filterId++;
   idList[0] = idList[1] = idList[2] = idList[3] = 0;
}

void Stm32Can::SetFilterBankMask(int& idIndex, int& filterId, uint16_t* idMaskList)
{
   if (idIndex > 0 && idIndex < IDS_PER_BANK)
   {
      //half-filled bank: duplicate pair 1 into pair 2, else the empty pair
      //(id 0, mask 0x7FF) phantom-matches exactly CAN ID 0x000
      idMaskList[2] = idMaskList[0];
      idMaskList[3] = idMaskList[1];
   }

   can_filter_id_mask_16bit_init(
         filterId,
         idMaskList[0] << 5, //id 1
         idMaskList[1] << 5, //mask 1
         idMaskList[2] << 5, //id 2
         idMaskList[3] << 5, //mask 2
         filterId & 1,
         true);
   idIndex = 0;
   filterId++;
   idMaskList[0] = idMaskList[2] = 0;
   idMaskList[1] = idMaskList[3] = 0x7FF;
}

void Stm32Can::SetFilterBank29(int& idIndex, int& filterId, uint32_t* idList)
{
   if (idIndex > 0 && idIndex < EXT_IDS_PER_BANK)
   {
      //half-filled bank: duplicate the single real entry
      idList[1] = idList[0];
   }

   can_filter_id_list_32bit_init(
         filterId,
         (idList[0] << 3) | 0x4, //filter extended
         (idList[1] << 3) | 0x4,
         filterId & 1,
         true);
   idIndex = 0;
   filterId++;
   idList[0] = idList[1] = 0;
}

void Stm32Can::PackFilters(const uint32_t* ids, const uint32_t* masks, int count, int filterId)
{
   uint16_t idList[IDS_PER_BANK] = { 0, 0, 0, 0 };
   uint16_t idMaskList[IDS_PER_BANK] = { 0, 0x7FF, 0, 0x7FF };
   uint32_t extIdList[EXT_IDS_PER_BANK] = { 0, 0 };
   int idIndex = 0, idMaskIndex = 0, extIdIndex = 0;

   for (int i = 0; i < count; i++)
   {
      if (ids[i] > 0x7ff)
      {
         extIdList[extIdIndex] = ids[i] & 0x1FFFFFFF;
         extIdIndex++;
      }
      else if (masks[i] != 0)
      {
         idMaskList[idMaskIndex++] = ids[i];
         idMaskList[idMaskIndex++] = masks[i];
      }
      else
      {
         idList[idIndex] = ids[i];
         idIndex++;
      }

      if (idIndex == IDS_PER_BANK)
      {
         SetFilterBank(idIndex, filterId, idList);
      }
      if (idMaskIndex == IDS_PER_BANK)
      {
         SetFilterBankMask(idMaskIndex, filterId, idMaskList);
      }
      if (extIdIndex == EXT_IDS_PER_BANK)
      {
         SetFilterBank29(extIdIndex, filterId, extIdList);
      }
   }

   //loop terminates before adding last set of filters
   if (idIndex > 0)
   {
      SetFilterBank(idIndex, filterId, idList);
   }
   if (idMaskIndex > 0)
   {
      SetFilterBankMask(idMaskIndex, filterId, idMaskList);
   }
   if (extIdIndex > 0)
   {
      SetFilterBank29(extIdIndex, filterId, extIdList);
   }
}

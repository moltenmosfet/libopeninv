/*
 * This file is part of the libopeninv project.
 *
 * Copyright (C) 2011 Johannes Huebner <dev@johanneshuebner.com>
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
#ifndef FOC_H
#define FOC_H

#include <stdint.h>
#include "my_fp.h"

class FOC
{
   public:
      static void SetAngle(uint16_t angle);
      static void ParkClarke(s32fp il1, s32fp il2);
      static int32_t GetQLimit(int32_t maxVd);
      static int32_t GetTotalVoltage(int32_t ud, int32_t uq);
      static void InvParkClarke(int32_t ud, int32_t uq, int32_t dtcomp = 0);
      static void Mtpa(float is, float& idref, float& iqref);
      static void SetMotorParameters(float lqminusld, float fluxLinkage);
      static int32_t GetMaximumModulationIndex();
      static void SetMaximumModulationIndex(uint32_t m);
      static s32fp id;
      static s32fp iq;
      static int32_t DutyCycles[3];

   protected:
   private:
      static uint32_t sqrt(uint32_t rad);
      static float floatSqrt(float rad);
      static int getexp(float f);
      static s32fp sin;
      static s32fp cos;
      /* F10/T5 dead-time compensation: sign of each phase's current (+1, -1, or
       * 0 inside the deadband), captured by ParkClarke from its (il1, il2)
       * arguments and consumed by InvParkClarke. phaseSign[i] lines up with
       * DutyCycles[i] because the caller (ProcessCurrents) already resolves the
       * SWAP_CURRENTS pinswap by choosing which measured current it passes as
       * il1 vs il2 -- ParkClarke itself needs no pinswap awareness. */
      static int8_t phaseSign[3];
};

#endif // FOC_H

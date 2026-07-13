/*
 * This file is part of the libopeninv project.
 *
 * Copyright (C) 2018 Johannes Huebner <dev@johanneshuebner.com>
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
#include "picontroller.h"
#include "my_math.h"

template<>
int32_t PiControllerGeneric<s32fp, int32_t>::Run(s32fp curVal, int32_t feedForward)
{
   s32fp err = refVal - curVal;
   esum += err;

   //anti windup
   esum = MIN(esum, maxSum);
   esum = MAX(esum, minSum);

   // Integrate in 64-bit: dividing esum by frequency before multiplying by ki
   // (as the naive form does) quantises the I-term to ki>>5-digit steps.
   int32_t iTerm = (int32_t)(((int64_t)esum * ki) / frequency);
   int32_t y = feedForward + FP_TOINT(err * kp + iTerm);
   int32_t ylim = MAX(y, minY);
   ylim = MIN(ylim, maxY);

   return ylim;
}

template<>
float PiControllerGeneric<float, float>::Run(float curVal, float feedForward)
{
   float err = refVal - curVal;
   esum += err;

   //anti windup
   esum = MIN(esum, maxSum);
   esum = MAX(esum, minSum);

   float y = feedForward + err * kp + (esum / frequency) * ki;
   float ylim = MAX(y, minY);
   ylim = MIN(ylim, maxY);

   return ylim;
}

template<>
int32_t PiControllerGeneric<s32fp, int32_t>::RunProportionalOnly(s32fp curVal)
{
   s32fp err = refVal - curVal;

   int32_t y = FP_TOINT(err * kp);
   int32_t ylim = MAX(y, minY);
   ylim = MIN(ylim, maxY);

   return ylim;
}

template<>
void PiControllerGeneric<s32fp, int32_t>::SetIntegralGain(int32_t ki)
{
    this->ki = ki;

    if (ki != 0)
    {
       // Compute the windup bounds in 64-bit and saturate: for small ki the
       // FP_FROMINT (<<5) overflowed int32, producing sign-flipped garbage.
       int64_t minS = (((int64_t)minY * frequency) / ABS(ki)) << FRAC_DIGITS;
       int64_t maxS = (((int64_t)maxY * frequency) / ABS(ki)) << FRAC_DIGITS;
       minSum = minS < INT32_MIN ? INT32_MIN : (minS > INT32_MAX ? INT32_MAX : minS);
       maxSum = maxS < INT32_MIN ? INT32_MIN : (maxS > INT32_MAX ? INT32_MAX : maxS);
    }
}

template<>
void PiControllerGeneric<float, float>::SetIntegralGain(float ki)
{
    this->ki = ki;

    if (ki != 0)
    {
       minSum = (minY * frequency) / ABS(ki);
       maxSum = (maxY * frequency) / ABS(ki);
    }
}

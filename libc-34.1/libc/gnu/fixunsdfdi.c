/* Subroutines needed by GCC output code on some machines.  */
/* Copyright (C) 1989 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, if you link this library with files
   compiled with GCC to produce an executable, this does not cause
   the resulting executable to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */

#include "gnulib.h"

#define WORD_SIZE (sizeof (long) * BITS_PER_UNIT)
#define HIGH_WORD_COEFF (((long long) 1) << WORD_SIZE)

long long _fixunsdfdi (double a)
{
  double b;
  unsigned long long v;

  if (a < 0)
    return 0;

  /* Compute high word of result, as a flonum.  */
  b = (a / HIGH_WORD_COEFF);
  /* Convert that to fixed (but not to long long!),
     and shift it into the high word.  */
  v = (unsigned long int) b;
  v <<= WORD_SIZE;
  /* Remove high part from the double, leaving the low part as flonum.  */
  a -= (double)v;
  /* Convert that to fixed (but not to long long!) and add it in.
     Sometimes A comes out negative.  This is significant, since
     A has more bits than a long int does.  */
  if (a < 0)
    v -= (unsigned long int) (- a);
  else
    v += (unsigned long int) a;
  return v;
}


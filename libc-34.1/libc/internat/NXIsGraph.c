/*
 * Copyright 1990, NeXT, Inc.
 */

#include	<NXCType.h>

int NXIsGraph(c)
	unsigned int c;
{
	return ((unsigned int)((_NX_CTypeTable_ + 1)[c] & (_P|_U|_L|_N)));
}

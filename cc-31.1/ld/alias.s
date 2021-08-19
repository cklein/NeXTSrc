| These pointers are declared in pointers.c.  They get filled in with a #init
| from the library specification file with the real values of their symbols.
| They are used here to make an indirect jmp to these routines.
.globl __librld_bcopy

| These are the routines to make the indirect jumps.   These are needed because
| they can't be substituted by cpp (because they are generated by the compiler
| directly).
.globl .librld_bcopy
.librld_bcopy:
	movel	__librld_bcopy, a0
	jmp	a0@

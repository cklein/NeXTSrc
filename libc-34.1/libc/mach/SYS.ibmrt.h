/* static char *rcsidSYS = "$Header: SYS.h,v 5.1 86/02/13 10:18:08 ibmacis Exp $"; */
/* $Source: /ibm/acis/usr/src/lib/libc/_ca/ca/sys/RCS/SYS.h,v $ */

#ifdef ONVAX
#include "/acis/usr/src/include/syscall.h"
#include "/acis/usr/src/sys/machine/scr.h"
#else
#include <machine/syscall.h>
#include <machine/scr.h>

#endif
#define D_NOFRAME 2	/* Should be defined from debug.h	*/
 
#ifdef PROF
#define ENTRY(x) .data; .align 2; .globl _/**/x;_/**/x: .long _./**/x,0;\
.text; .globl _./**/x; _./**/x: stm r14,ARG3_OFFSET(r1); ai r1,-REG_SAVE_SZ;\
get r14,$_/**/x; mr r0,r15 ; bali r15,mcount; ai r1,REG_SAVE_SZ; lm r14,ARG3_OFFSET(r1);
#else
#define ENTRY(x) .data; .align 2; .globl _/**/x;_/**/x: .long _./**/x;\
.text; .globl _./**/x; _./**/x:; 
#endif

#define SYSCALL(x) ENTRY(x); SVC(x) ; bntbr r15;	/* no-error return */\
.globl _errno; get r5,$_errno; sts r2,0(r5); get r2,$-1; br r15; TTNOFRM
#define SYSCODE(x) ENTRY(x)
#define SVC(x) get r0,0(sp); svc SYS_/**/x(r0)
#define TTNOFRM .align 2; .byte 0xdf, D_NOFRAME, 0xdf, 0;
.globl .oVncs; .set .oVncs,0 

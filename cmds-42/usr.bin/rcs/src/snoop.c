/*
 *                     Logging of RCS commands co and ci
 */
#ifndef lint
 static char rcsid[]=
 "$Header: /usr/src/local/bin/rcs/src/RCS/snoop.c,v 4.4 89/05/01 15:14:00 narten Exp $ Purdue CS";
#endif
/*******************************************************************
 * This program appends argv[1] to the file SNOOPFILE.
 * To avoid overlaps, it creates a lockfile with name lock in the same
 * directory as SNOOPFILE. SNOOPFILE must be defined in the cc command. 
 * Prints an error message if lockfile doesn't get deleted after
 * MAXTRIES tries.
 *******************************************************************
 */

/* Copyright (C) 1982, 1988, 1989 Walter Tichy
   Distributed under license by the Free Software Foundation, Inc.

This file is part of RCS.

RCS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

RCS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RCS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

Report problems and direct all questions to:

    rcs-bugs@cs.purdue.edu

*/


/* $Log:	snoop.c,v $
 * Revision 4.4  89/05/01  15:14:00  narten
 * changed copyright header to reflect current distribution rules
 * 
 * Revision 4.3  87/12/18  11:46:52  narten
 * more lint cleanups (Guy Harris)
 * 
 * Revision 4.2  87/10/18  10:41:47  narten
 * Changing version numbers. Changes relative to 1.1 actually relative to 
 * 4.1
 * 
 * Revision 1.2  87/09/24  14:01:41  narten
 * Sources now pass through lint (if you ignore printf/sprintf/fprintf 
 * warnings)
 * 
 * Revision 1.1  84/01/23  14:50:49  kcs
 * Initial revision
 * 
 * Revision 4.1  83/03/28  13:23:42  wft
 * No change; just new revision number.
 * 
 * Revision 3.2  82/12/04  17:14:31  wft
 * Added rcsbase.h, changed SNOOPDIR to SNOOPFILE, reintroduced
 * error message in case of permanent locking.
 * 
 * Revision 3.1  82/10/18  21:22:03  wft
 * Number of polls now 20, no error message if critical section can't
 * be entered.
 * 
 * Revision 2.3  82/07/01  23:49:28  wft
 * changed copyright notice only.
 * 
 * Revision 2.2  82/06/03  20:00:10  wft
 * changed name from rcslog to snoop, replaced LOGDIR with SNOOPDIR.
 * 
 * Revision 2.1  82/05/06  17:55:54  wft
 * Initial revision
 *
 */


#include "rcsbase.h"
#define fflsbuf _flsbuf
/* undo redefinition of putc in rcsbase.h */

char  lockfname[NCPPN];
FILE * logfile;
int lockfile;

#define MAXTRIES 20

main(argc,argv)
int argc; char * argv[];
/* writes argv[1] to SNOOPFILE and appends a newline. Invoked as follows:
 * rcslog logmessage
 */
{       int tries;
        register char * lastslash, *sp;

        VOID strcpy(lockfname,(char *) SNOOPFILE);
        lastslash = sp = lockfname;
        while (*sp) if (*sp++ =='/') lastslash=sp; /* points beyond / */
        VOID strcpy(lastslash,",lockfile");
        tries=0;
        while (((lockfile=creat(lockfname, 000)) == -1) && (tries<=MAXTRIES)) {
                tries++;
                sleep(5);
        }
        if (tries<=MAXTRIES) {
                VOID close(lockfile);
                if ((logfile=fopen(SNOOPFILE,"a")) ==NULL) {
                        VOID fprintf(stderr,"Can't open logfile %s\n",SNOOPFILE);
                } else {
                        VOID fputs(argv[1],logfile);
                        VOID putc('\n',logfile);
                        VOID fclose(logfile);
                }
                VOID unlink(lockfname);
        } else {
                VOID fprintf(stderr,"RCS logfile %s seems permanently locked.\n",SNOOPFILE);
                VOID fprintf(stderr,"Please alert system administrator\n");
        }
}

/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/* 
 * Copyright (c) 1984 by Sun Microsystems, Inc.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getprotoent.c	1.3 88/03/31 4.0NFSSRC; from	5.3 (Berkeley) 5/19/86"; /* and from 1.17 88/02/08 SMI Copyr 1984 Sun Micro */
#endif LIBC_SCCS and not lint

#if	NeXT
#include <stdlib.h>
#endif	NeXT
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <rpcsvc/ypclnt.h>

/*
 * Internet version.
 */
static struct protodata {
	FILE	*protof;
	char	*current;
	int	currentlen;
	int	stayopen;
#define	MAXALIASES	35
	char	*proto_aliases[MAXALIASES];
	struct	protoent proto;
	char	line[BUFSIZ];
	char	*domain;
	char	*map;
	int	usingyellow;
} *protodata, *_protodata();

#if	NeXT
static yellowup(void);
#endif	NeXT
static	struct protoent *interpret();
struct	protoent *_old_getprotoent();
char	*inet_ntoa();
static	char *any();
static	char PROTODB[] = "/etc/protocols";

static struct protodata *
_protodata()
{
	register struct protodata *d = protodata;

	if (d == 0) {
		d = (struct protodata *)calloc(1, sizeof (struct protodata));
		protodata = d;
	}
	return (d);
}

struct protoent *
_old_getprotobynumber(proto)
{
	register struct protodata *d = _protodata();
	register struct protoent *p;
	int reason;
	char adrstr[12], *val = NULL;
	int vallen;

	if (d == 0)
		return (0);
	d->map = "protocols.bynumber";
	_old_setprotoent(0);
	if (!d->usingyellow) {
		while (p = _old_getprotoent()) {
			if (p->p_proto == proto)
				break;
		}
	}
	else {
		sprintf(adrstr, "%d", proto);
		if (reason = yp_match(d->domain, d->map,
		    adrstr, strlen(adrstr), &val, &vallen)) {
#ifdef DEBUG
			fprintf(stderr, "reason yp_match failed is %d\n",
			    reason);
#endif
			p = NULL;
		    }
		else {
			p = interpret(val, vallen);
			free(val);
		}
	}
	_old_endprotoent();
	return (p);
}

struct protoent *
_old_getprotobyname(name)
	register char *name;
{
	register struct protodata *d = _protodata();
	register struct protoent *p;
	register char **cp;
	int reason;
	char *val = NULL;
	int vallen;

	if (d == 0)
		return (0);
	d->map = "protocols.byname";
	_old_setprotoent(0);
	if (!d->usingyellow) {
		while (p = _old_getprotoent()) {
			if (strcmp(p->p_name, name) == 0)
				break;
			for (cp = p->p_aliases; *cp != 0; cp++)
				if (strcmp(*cp, name) == 0)
					goto found;
		}
	}
	else {
		if (reason = yp_match(d->domain, d->map,
		    name, strlen(name), &val, &vallen)) {
#ifdef DEBUG
			fprintf(stderr, "reason yp_match failed is %d\n",
			    reason);
#endif
			p = NULL;
		    }
		else {
			p = interpret(val, vallen);
			free(val);
		}
	}
found:
	_old_endprotoent();
	return (p);
}

_old_setprotoent(f)
	int f;
{
	register struct protodata *d = _protodata();
	
	if (d == 0)
		return;
	if (d->protof == NULL)
		d->protof = fopen(PROTODB, "r");
	else
		rewind(d->protof);
	if (d->current)
		free(d->current);
	d->current = NULL;
	d->stayopen |= f;
	if (d->map) yellowup();	/* recompute whether yellow pages are up */
}

_old_endprotoent()
{
	register struct protodata *d = _protodata();

	if (d == 0)
		return;
	if (d->current && !d->stayopen) {
		free(d->current);
		d->current = NULL;
	}
	if (d->protof && !d->stayopen) {
		fclose(d->protof);
		d->protof = NULL;
	}
}

struct protoent *
_old_getprotoent()
{
	register struct protodata *d = _protodata();
	int reason;
	char *key = NULL, *val = NULL;
	int keylen, vallen;
	struct protoent *pp;

	if (d == 0)
		return (0);
	d->map = "protocols.bynumber";
	yellowup();
	if (!d->usingyellow) {
		if (d->protof == NULL && (d->protof = fopen(PROTODB, "r")) == NULL)
			return (NULL);
	        if (fgets(d->line, BUFSIZ, d->protof) == NULL)
			return (NULL);
		return interpret(d->line, strlen(d->line));
	}
	if (d->current == NULL) {
		if (reason =  yp_first(d->domain, d->map,
		    &key, &keylen, &val, &vallen)) {
#ifdef DEBUG
			fprintf(stderr, "reason yp_first failed is %d\n",
			    reason);
#endif
			return NULL;
		    }
	}
	else {
		if (reason = yp_next(d->domain, d->map,
		    d->current, d->currentlen, &key, &keylen, &val, &vallen)) {
#ifdef DEBUG
			fprintf(stderr, "reason yp_next failed is %d\n",
			    reason);
#endif
			return NULL;
		}
	}
	if (d->current)
		free(d->current);
	d->current = key;
	d->currentlen = keylen;
	pp = interpret(val, vallen);
	free(val);
	return (pp);
}

static struct protoent *
interpret(val, len)
{
	register struct protodata *d = _protodata();
	char *p;
	register char *cp, **q;

	if (d == NULL)
		return (0);
	strncpy(d->line, val, len);
	p = d->line;
	d->line[len] = '\n';
	if (*p == '#')
		return (getprotoent());
	cp = any(p, "#\n");
	if (cp == NULL)
		return (getprotoent());
	*cp = '\0';
	d->proto.p_name = p;
	cp = any(p, " \t");
	if (cp == NULL)
		return (getprotoent());
	*cp++ = '\0';
	while (*cp == ' ' || *cp == '\t')
		cp++;
	p = any(cp, " \t");
	if (p != NULL)
		*p++ = '\0';
	d->proto.p_proto = atoi(cp);
	q = d->proto.p_aliases = d->proto_aliases;
	if (p != NULL) {
		cp = p;
		while (cp && *cp) {
			if (*cp == ' ' || *cp == '\t') {
				cp++;
				continue;
			}
			if (q < &(d->proto_aliases[MAXALIASES - 1]))
				*q++ = cp;
			cp = any(cp, " \t");
			if (cp != NULL)
				*cp++ = '\0';
		}
	}
	*q = NULL;
	return (&d->proto);
}

/*
 * check to see if yellow pages are up, and store that fact in usingyellow.
 */
static
yellowup()
{
	register struct protodata *d = _protodata();

	if (d == 0)
		return;
	if (d->domain == NULL) {
		d->usingyellow = usingypmap(&d->domain, d->map);
	}
}

static char *
any(cp, match)
	register char *cp;
	char *match;

{
	register char *mp, c;

	while (c = *cp) {
		for (mp = match; *mp; mp++)
			if (*mp == c)
				return (cp);
		cp++;
	}
	return ((char *)0);
}

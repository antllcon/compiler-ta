#include "nfa_c_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Convert infix regexp re to postfix notation.
 */
char* re2post(char* re)
{
	int nalt, natom;
	static char buf[8000];
	char* dst;
	struct
	{
		int nalt;
		int natom;
	} paren[100], *p;

	p = paren;
	dst = buf;
	nalt = 0;
	natom = 0;
	if (strlen(re) >= sizeof buf / 2)
		return NULL;
	for (; *re; re++)
	{
		switch (*re)
		{
		case '(':
			if (natom > 1)
			{
				--natom;
				*dst++ = '.';
			}
			if (p >= paren + 100)
				return NULL;
			p->nalt = nalt;
			p->natom = natom;
			p++;
			nalt = 0;
			natom = 0;
			break;
		case '|':
			if (natom == 0)
				return NULL;
			while (--natom > 0)
				*dst++ = '.';
			nalt++;
			break;
		case ')':
			if (p == paren)
				return NULL;
			if (natom == 0)
				return NULL;
			while (--natom > 0)
				*dst++ = '.';
			for (; nalt > 0; nalt--)
				*dst++ = '|';
			--p;
			nalt = p->nalt;
			natom = p->natom;
			natom++;
			break;
		case '*':
		case '+':
		case '?':
			if (natom == 0)
				return NULL;
			*dst++ = *re;
			break;
		default:
			if (natom > 1)
			{
				--natom;
				*dst++ = '.';
			}
			*dst++ = *re;
			natom++;
			break;
		}
	}
	if (p != paren)
		return NULL;
	while (--natom > 0)
		*dst++ = '.';
	for (; nalt > 0; nalt--)
		*dst++ = '|';
	*dst = 0;
	return buf;
}

enum
{
	Match = 256,
	Split = 257
};

struct State
{
	int c;
	CState* out;
	CState* out1;
	int lastlist;
};

CState matchstate = {Match}; /* matching state */
int nstate;

/* Allocate and initialize CState */
CState* state(int c, CState* out, CState* out1)
{
	CState* s;

	nstate++;
	s = (CState*)malloc(sizeof *s);
	s->lastlist = 0;
	s->c = c;
	s->out = out;
	s->out1 = out1;
	return s;
}

/*
 * ... (Frag и Ptrlist) ...
 */
typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag
{
	CState* start;
	Ptrlist* out;
};

/* Initialize Frag struct. */
Frag frag(CState* start, Ptrlist* out)
{
	Frag n = {start, out};
	return n;
}

union Ptrlist
{
	Ptrlist* next;
	CState* s;
};

/* Create singleton list containing just outp. */
Ptrlist* list1(CState** outp)
{
	Ptrlist* l;

	l = (Ptrlist*)outp;
	l->next = NULL;
	return l;
}

/* Patch the list of states at out to point to start. */
void patch(Ptrlist* l, CState* s)
{
	Ptrlist* next;

	for (; l; l = next)
	{
		next = l->next;
		l->s = s;
	}
}

/* Join the two lists l1 and l2, returning the combination. */
Ptrlist* append(Ptrlist* l1, Ptrlist* l2)
{
	Ptrlist* oldl1;

	oldl1 = l1;
	while (l1->next)
		l1 = l1->next;
	l1->next = l2;
	return oldl1;
}

/*
 * Convert postfix regular expression to NFA.
 * Return start state.
 */
CState* post2nfa(char* postfix)
{
	char* p;
	Frag stack[1000], *stackp, e1, e2, e;
	CState* s;

	if (postfix == NULL)
		return NULL;

#define push(s) *stackp++ = s
#define pop() *--stackp

	stackp = stack;
	for (p = postfix; *p; p++)
	{
		switch (*p)
		{
		default:
			s = state(*p, NULL, NULL);
			push(frag(s, list1(&s->out)));
			break;
		case '.': /* catenate */
			e2 = pop();
			e1 = pop();
			patch(e1.out, e2.start);
			push(frag(e1.start, e2.out));
			break;
		case '|': /* alternate */
			e2 = pop();
			e1 = pop();
			s = state(Split, e1.start, e2.start);
			push(frag(s, append(e1.out, e2.out)));
			break;
		case '?': /* zero or one */
			e = pop();
			s = state(Split, e.start, NULL);
			push(frag(s, append(e.out, list1(&s->out1))));
			break;
		case '*': /* zero or more */
			e = pop();
			s = state(Split, e.start, NULL);
			patch(e.out, s);
			push(frag(s, list1(&s->out1)));
			break;
		case '+': /* one or more */
			e = pop();
			s = state(Split, e.start, NULL);
			patch(e.out, s);
			push(frag(e.start, list1(&s->out1)));
			break;
		}
	}

	e = pop();
	if (stackp != stack)
		return NULL;

	patch(e.out, &matchstate);
	return e.start;
#undef pop
#undef push
}
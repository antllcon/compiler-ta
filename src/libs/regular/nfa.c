#include "nfa_c_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CODE_ANY   (char)0x80
#define CODE_START (char)0x81
#define CODE_END   (char)0x82
#define CONCAT_OP 0x1F

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
				*dst++ = CONCAT_OP;
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
				*dst++ = CONCAT_OP;
			nalt++;
			break;

		case ')':
			if (p == paren)
				return NULL;
			if (natom == 0)
				return NULL;
			while (--natom > 0)
				*dst++ = CONCAT_OP;
			for (; nalt > 0; nalt--)
				*dst++ = '|';
			--p;
			nalt = p->nalt;
			natom = p->natom;
			natom++;
			break;

		case '^':
			if (natom > 1)
			{
				--natom;
				*dst++ = CONCAT_OP;
			}
			*dst++ = CODE_START;
			natom++;
			break;

		case '$':
			if (natom > 1)
			{
				--natom;
				*dst++ = CONCAT_OP;
			}
			*dst++ = CODE_END;
			natom++;
			break;

		case '.':
			if (natom > 1)
			{
				--natom;
				*dst++ = CONCAT_OP;
			}
			*dst++ = CODE_ANY;
			natom++;
			break;

		case '*':
		case '+':
		case '?':
			if (natom == 0)
				return NULL;
			*dst++ = *re;
			break;

		case '\\':
			if (*(re + 1))
			{
				if (natom > 1)
				{
					--natom;
					*dst++ = CONCAT_OP;
				}
				re++;
				*dst++ = *re;
				natom++;
			}
			break;

		default:
			if (natom > 1)
			{
				--natom;
				*dst++ = CONCAT_OP;
			}
			*dst++ = *re;
			natom++;
			break;
		}
	}
	if (p != paren)
		return NULL;
	while (--natom > 0)
		*dst++ = CONCAT_OP;
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

CState matchstate = {Match};
int nstate;

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

typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag
{
	CState* start;
	Ptrlist* out;
};

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

Ptrlist* list1(CState** outp)
{
	Ptrlist* l;

	l = (Ptrlist*)outp;
	l->next = NULL;
	return l;
}

void patch(Ptrlist* l, CState* s)
{
	Ptrlist* next;

	for (; l; l = next)
	{
		next = l->next;
		l->s = s;
	}
}

Ptrlist* append(Ptrlist* l1, Ptrlist* l2)
{
	Ptrlist* oldl1;

	oldl1 = l1;
	while (l1->next)
		l1 = l1->next;
	l1->next = l2;
	return oldl1;
}

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

		case CODE_ANY:
			s = state(CODE_ANY, NULL, NULL);
			push(frag(s, list1(&s->out)));
			break;

		case CODE_START:
			s = state(CODE_START, NULL, NULL);
			push(frag(s, list1(&s->out)));
			break;

		case CODE_END:
			s = state(CODE_END, NULL, NULL);
			push(frag(s, list1(&s->out)));
			break;

		case CONCAT_OP:
			e2 = pop();
			e1 = pop();
			patch(e1.out, e2.start);
			push(frag(e1.start, e2.out));
			break;

		case '|':
			e2 = pop();
			e1 = pop();
			s = state(Split, e1.start, e2.start);
			push(frag(s, append(e1.out, e2.out)));
			break;

		case '?':
			e = pop();
			s = state(Split, e.start, NULL);
			push(frag(s, append(e.out, list1(&s->out1))));
			break;

		case '*':
			e = pop();
			s = state(Split, e.start, NULL);
			patch(e.out, s);
			push(frag(s, list1(&s->out1)));
			break;

		case '+':
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
/* parser of bf source */

#include "bf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int is_bf_char(int ch)
{
	return ch && strchr(",.+-<>[]", ch) != 0;
}

static int fgetbfc(FILE *f)
{
	int ch;

	if (feof(f)) return EOF;
	do {
		ch = fgetc(f);
	} while (ch != EOF && !is_bf_char(ch));
	return ch;
}


#ifdef BF_LOOP_OPTIMIZE
static int muladd_applicable(struct bfop *s, struct bfop *t)
{
	int b;
	int delta;

	b = 0;
	delta = 0;
	while (++s < t) {
		switch (s->type) {
			case LEFT: b -= s->repeat; break;
			case RIGHT: b += s->repeat; break;
			case INC: if (b == 0) delta += s->repeat; break;
			case DEC: if (b == 0) delta -= s->repeat; break;
			case LEND:
				   bfi_fatal("Parser bug: LEND is not allowed in muladd_applicable()");
				   break;
			default: return 0;	// not applicable
		}
	}
	return b == 0 && delta == -1;
}

static struct bfop *
optimize_loop(struct bfop *s, struct bfop *t)
{
	assert(s->type == LOOP && t->type == LEND);
	if (muladd_applicable(s, t)) {
		struct bfop *p;
		int offset;

		p = s + 1;
		if (p[0].type == INC || p[0].type == DEC)
			++p;	// first op is INC/DEC in current cell; ignore it.
		assert(p <= t);
		offset = 0;
		while (p < t) {
			assert(p[0].type == LEFT || p[0].type == RIGHT);
#define REF_DIR_VAL(op, dir) (((op).type == (dir)) ? (op).repeat : -(op).repeat)
			offset += REF_DIR_VAL(p[0], RIGHT);
			if (offset) {
				assert(p[1].type == INC || p[1].type == DEC);
				s->type = MULADD;
				s->repeat = REF_DIR_VAL(p[1], INC);
				s->offset = offset;
				++s;
			}
			p += 2;
		}
		assert(offset == 0);
#undef REF_DIR_VAL
		// do not forget to clear the value in the current cell
		s->type = CLEAR;
		s->repeat = 1;
		s->offset = 0;
		++s;
		assert(s <= t);
		memset(s, 0, sizeof (struct bfop) * (t-s+1));
		return s;
	}
	return t;
}
#endif

#ifdef BF_COLLECT_OPTIMIZE
static int counter_op(int ch)
{
	char *p;
	
	return (ch > 0 && (p = strchr("+-+<><", ch))) ? p[1] : 0;
}
#endif

struct bfop *
bf_parse(struct bfop *pc, struct bfop *lim, FILE *src)
{
	int op;
#ifdef BF_COLLECT_OPTIMIZE
	int op1;
#endif

	if (pc >= lim) bfi_fatal("Parser ran out of memory.");
	for (;;) {
		struct bfop *pc1;

		op = fgetbfc(src);
#ifdef BF_COLLECT_OPTIMIZE
		op1 = counter_op(pc->type);	// collectable op has a counter op
		if (op1 && op == pc->type)
			++pc->repeat;
		else if (op == op1)
			--pc->repeat;
		else {	// append new op at pc
			if (pc->type && pc->repeat)
				++pc;
			pc->type = op;
			pc->repeat = 1;
		}
#else
		if (pc->type)
			++pc;	// we can safely overwrite when pc->type == 0 (no op)
		pc->type = op;
#endif
		switch (op) {
		case EOF:
			pc->type = 0;	// let pc->type be 0 (no op) instead of -1 (EOF)
			/*@fallthrough@*/
		case LEND:
			return pc;
		case LOOP:
			pc1 = bf_parse(pc+1, lim, src);
			if (pc1->type != LEND)
				bfi_fatal("expect ]");
#ifdef BF_COLLECT_OPTIMIZE
			// repeat field for [ and ] means the jumping distance
			pc1->repeat = pc->repeat = (int) (pc1-pc);
#ifdef BF_LOOP_OPTIMIZE
			pc1 = optimize_loop(pc, pc1);
#endif /* BF_LOOP_OPTIMIZE */
#endif /* BF_COLLECT_OPTIMIZE */
			pc = pc1;
			break;
		}
		//fprintf(stderr, "LOG: pc=%p\n", pc);
	}
}

void bfi_fatal(const char *msg)
{
	fprintf(stderr, "FATAL: %s\n", msg);
	exit(EXIT_FAILURE);
}
